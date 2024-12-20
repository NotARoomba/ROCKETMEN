use dotenv::dotenv;
use futures::future::ok;
use futures::{ channel::mpsc::{ unbounded, UnboundedSender }, pin_mut, StreamExt, TryStreamExt };
use futures::future::select;
use serde::{ Deserialize, Serialize };
use std::env;
use std::{ collections::HashMap, net::SocketAddr, sync::{ Arc, Mutex } };
use tokio::net::{ TcpListener, TcpStream };
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
    temperature: f32,
    pressure: f32,
    time: i64,
}

async fn handle_connection(peer_map: PeerMap, addr: SocketAddr, stream: TcpStream) {
    println!("Incoming TCP connection from: {}", addr);

    let ws_stream = tokio_tungstenite::accept_async(stream).await;
    if ws_stream.is_err() {
        println!("Error during the websocket handshake occurred");
        return;
    }
    println!("WebSocket connection established: {}", addr);

    // Insert the write part of this peer to the peer map.
    let (tx, rx) = unbounded();
    peer_map.lock().expect("Error Locking").insert(addr, tx);

    let (outgoing, incoming) = ws_stream.unwrap().split();

    let broadcast_incoming = incoming.try_for_each(|msg| {
        println!("Received a message from {}: {}", addr, msg.to_text().unwrap());
        let mut data = serde_json::from_str::<ClientData>(msg.to_text().unwrap()).unwrap();
        if data.auth_token != env::var("AUTH_TOKEN").expect("AUTH_TOKEN must be set.") {
            return ok(());
        }
        data.auth_token = "REDACTED".to_string();
        println!("Data: {:#?}", data);
        let peers = peer_map.lock().unwrap();

        // We want to broadcast the message to everyone except ourselves.
        let broadcast_recipients = peers
            .iter()
            .filter(|(peer_addr, _)| peer_addr != &&addr)
            .map(|(_, ws_sink)| ws_sink);

        for recp in broadcast_recipients {
            recp.unbounded_send(
                Message::text(serde_json::to_string(&data.clone()).unwrap())
            ).unwrap();
        }

        ok(())
    });

    let receive_from_others = rx.map(Ok).forward(outgoing);

    pin_mut!(broadcast_incoming, receive_from_others);
    select(broadcast_incoming, receive_from_others).await;

    println!("{} disconnected", &addr);
    peer_map.lock().unwrap().remove(&addr);
}

#[tokio::main]
async fn main() {
    env_logger::init();
    dotenv().ok();

    let addr = "127.0.0.1:3001";
    let listener = TcpListener::bind(&addr).await.expect("Can't listen");
    println!("Listening on: {}", addr);

    let state = PeerMap::new(Mutex::new(HashMap::new()));

    while let Ok((stream, _)) = listener.accept().await {
        let peer = stream.peer_addr().expect("Connected streams should have a peer address");
        println!("Peer address: {}", peer);

        tokio::spawn(handle_connection(state.clone(), peer, stream));
    }
}
