use dotenv::dotenv;
use futures::future::ok;
use futures::{ channel::mpsc::{ unbounded, UnboundedSender }, pin_mut, StreamExt, TryStreamExt };
use futures::future::select;
use hyper::body::{ Bytes, Incoming };
use hyper::{ Error, Request, Response };
use hyper_util::rt::TokioIo;
use serde::{ Deserialize, Serialize };
use std::env;
use std::{ collections::HashMap, net::SocketAddr, sync::{ Arc, Mutex } };
use tokio::net::TcpListener;
use http_body_util::Full;
use tokio_tungstenite::tungstenite::Message;

type Tx = UnboundedSender<Message>;
type PeerMap = Arc<Mutex<HashMap<SocketAddr, Tx>>>;

#[derive(Clone, Debug, Serialize, Deserialize)]
struct ClientData {
    auth_token: String,
    angle_x: f32,
    angle_y: f32,
    angle_z: f32,
    accel_x: f32,
    accel_y: f32,
    accel_z: f32,
    avg_accel: f32,
    vel_x: f32,
    vel_y: f32,
    vel_z: f32,
    avg_vel: f32,
    pos_x: f32,
    pos_y: f32,
    pos_z: f32,
    temperature: f32,
    pressure: f32,
    time: i64,
}

async fn handle_connection(
    peer_map: PeerMap,
    addr: SocketAddr,
    mut request: Request<Incoming>
) -> Result<Response<Full<Bytes>>, Error> {
    tracing::info!("Incoming TCP connection from: {}", addr);

    if hyper_tungstenite::is_upgrade_request(&request) {
        let (response, stream) = hyper_tungstenite
            ::upgrade(&mut request, None)
            .expect("Upgrade failed");

        // Spawn a task to handle the websocket connection.
        tokio::spawn(async move {
            let ws_stream = stream.await;
            // let ws_stream = tokio_tungstenite::accept_async(stream).await;
            if ws_stream.is_err() {
                tracing::info!("Error during the websocket handshake occurred");
                return;
            }
            tracing::info!("WebSocket connection established: {}", addr);

            // Insert the write part of this peer to the peer map.
            let (tx, rx) = unbounded();
            peer_map.lock().expect("Error Locking").insert(addr, tx);

            let (outgoing, incoming) = ws_stream.unwrap().split();

            let broadcast_incoming = incoming.try_for_each(|msg| {
                if !msg.is_text() {
                    return ok(());
                }
                let mut data = serde_json::from_str::<ClientData>(msg.to_text().unwrap()).unwrap();
                if data.auth_token != env::var("AUTH_TOKEN").expect("AUTH_TOKEN must be set.") {
                    return ok(());
                }
                data.auth_token = "REDACTED".to_string();
                tracing::info!("Data: {:#?}", data);
                let peers = peer_map.lock().unwrap();

                // We want to broadcast the message to everyone except ourselves.
                let broadcast_recipients = peers
                    .iter()
                    .filter(|(peer_addr, _)| peer_addr != &&addr)
                    .map(|(_, ws_sink)| ws_sink);

                for recp in broadcast_recipients {
                    let _ = recp.unbounded_send(
                        Message::text(serde_json::to_string(&data.clone()).unwrap())
                    );
                }

                ok(())
            });

            let receive_from_others = rx.map(Ok).forward(outgoing);

            pin_mut!(broadcast_incoming, receive_from_others);
            select(broadcast_incoming, receive_from_others).await;

            tracing::info!("{} disconnected", &addr);
            peer_map.lock().unwrap().remove(&addr);
        });

        // Return the response so the spawned future can continue.
        Ok(response)
    } else {
        // Handle regular HTTP requests here.
        Ok(Response::new(Full::<Bytes>::from("You are not supposed to be here!")))
    }
}

#[tokio::main]
async fn main() {
    tracing_subscriber::fmt::init();
    dotenv().ok();

    let port: u16 = std::env
        ::var("PORT")
        .unwrap_or("8080".into())
        .parse()
        .expect("failed to convert to number");

    let addr = SocketAddr::from(([0, 0, 0, 0, 0, 0, 0, 0], port));

    let listener = TcpListener::bind(&addr).await.expect("Can't listen");
    tracing::info!("Listening on: {}", addr);

    let mut http = hyper::server::conn::http1::Builder::new();
    http.keep_alive(true);

    let state = PeerMap::new(Mutex::new(HashMap::new()));

    while let Ok((stream, _)) = listener.accept().await {
        let peer = stream.peer_addr().expect("Connected streams should have a peer address");
        tracing::info!("Peer address: {}", peer);
        let state = state.clone();
        let connection = http
            .serve_connection(
                TokioIo::new(stream),
                hyper::service::service_fn(move |req| {
                    handle_connection(state.to_owned(), peer, req)
                })
            )
            .with_upgrades();
        tokio::spawn(async move {
            if let Err(err) = connection.await {
                tracing::info!("Error serving HTTP connection: {err:?}");
            }
        });
    }
}
