use serde::Serialize;
use serde_json::json;
use serialport;
use dotenv::dotenv;
use std::{ io::{ BufRead, BufReader }, time::Duration };
use tokio_tungstenite::{ connect_async, tungstenite::protocol::Message };
use futures::SinkExt;

#[derive(Debug, Serialize)]
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

#[tokio::main]
async fn main() {
    dotenv().ok();
    rustls::crypto::ring
        ::default_provider()
        .install_default()
        .expect("Failed to install default rustls crypto provider");
    println!("Talos Client");

    let api_url: String = std::env::var("API_URL").expect("API_URL must be set.");
    let auth_token: String = std::env::var("AUTH_TOKEN").expect("AUTH_TOKEN must be set.");

    let ports = serialport::available_ports().expect("No ports found!");
    if ports.len() == 0 {
        println!("No ports found!");
    } else if ports.len() == 1 {
        println!("Found 1 port: {}", ports[0].port_name);
        let port = serialport
            ::new(ports[0].port_name.clone(), 115_200)
            .timeout(Duration::from_millis(1000))
            .open()
            .expect("Failed to open port");
        println!("Port opened successfully");
        let mut received_text = String::new();
        let mut reader = BufReader::new(port);
        let mut data: Vec<f32>;
        let mut current_data: ClientData = ClientData {
            auth_token,
            angle_x: 0.0,
            angle_y: 0.0,
            angle_z: 0.0,
            accel_x: 0.0,
            accel_y: 0.0,
            accel_z: 0.0,
            avg_accel: 0.0,
            vel_x: 0.0,
            vel_y: 0.0,
            vel_z: 0.0,
            avg_vel: 0.0,
            pos_x: 0.0,
            pos_y: 0.0,
            pos_z: 0.0,
            temperature: 0.0,
            pressure: 0.0,
            time: 0,
        };

        let (mut ws_stream, _) = connect_async(api_url).await.expect("Failed to connect");

        loop {
            // Read data from the serial port

            // Data was received, convert it to a string and print it
            // println!("Received: {}", received_text.trim());
            data = loop {
                received_text.clear();
                let err = reader.read_line(&mut received_text);
                if err.is_err() {
                    current_data = ClientData {
                        auth_token: current_data.auth_token.clone(),
                        angle_x: 0.0,
                        angle_y: 0.0,
                        angle_z: 0.0,
                        accel_x: 0.0,
                        accel_y: 0.0,
                        accel_z: 0.0,
                        avg_accel: 0.0,
                        vel_x: 0.0,
                        vel_y: 0.0,
                        vel_z: 0.0,
                        avg_vel: 0.0,
                        pos_x: 0.0,
                        pos_y: 0.0,
                        pos_z: 0.0,
                        temperature: 0.0,
                        pressure: 0.0,
                        time: current_data.time, // keep the last time
                    };
                    println!("Error reading from serial port: {:?}", err.err());
                    continue;
                }
                let parsed: Result<Vec<f32>, ()> = received_text
                    .clone()
                    .trim()
                    .split(',')
                    .map(|x| {
                        match x.trim().parse::<f32>() {
                            Ok(val) => Ok(val),
                            Err(e) => {
                                println!("Error parsing data: {:?}, for number {:?}", e, x);
                                Err(())
                            }
                        }
                    })
                    .collect();

                match parsed {
                    Ok(vec) if vec.len() == 8 => {
                        break vec;
                    } // valid data and correct length
                    Ok(vec) => {
                        println!("Invalid length: expected 8 values, got {}", vec.len());
                        continue;
                    }
                    Err(_) => {
                        println!("Retrying due to parsing error...");
                        continue;
                    }
                }
            };
            if current_data.time == 0 {
                println!("Setting time");
                current_data.time = chrono::Utc::now().timestamp_micros();
            }
            let dt =
                ((chrono::Utc::now().timestamp_micros() - current_data.time) as f32) / 1000000.0;

            println!("dt: {}", dt);
            // match received_text.split(":").collect::<Vec<&str>>()[0] {
            //     "ACCEL" => {
            //         data[0] = data[0] / 1000.0;
            //         data[1] = data[1] / 1000.0;
            //         data[2] = data[2] / 1000.0;

            //         current_data.accel_x = data[0] * (dt as f32);
            //         current_data.accel_y = data[1] * (dt as f32);
            //         current_data.accel_z = data[2] * (dt as f32);
            //         current_data.avg_accel = f32::sqrt(
            //             current_data.accel_x.powi(2) +
            //                 current_data.accel_y.powi(2) +
            //                 current_data.accel_z.powi(2)
            //         );
            //         current_data.vel_x = current_data.accel_x * dt;
            //         current_data.vel_y = current_data.accel_y * dt;
            //         current_data.vel_z = current_data.accel_z * dt;
            //         current_data.avg_vel = f32::sqrt(
            //             current_data.vel_x.powi(2) +
            //                 current_data.vel_y.powi(2) +
            //                 current_data.vel_z.powi(2)
            //         );
            //     }
            //     "ANGLE" => {
            //         data[0] = data[0] / 1000.0;
            //         data[1] = data[1] / 1000.0;
            //         data[2] = data[2] / 1000.0;

            //         current_data.angle_x += data[0] * (dt as f32);
            //         current_data.angle_y += data[1] * (dt as f32);
            //         current_data.angle_z += data[2] * (dt as f32);
            //     }
            //     "TEMP" => {
            //         current_data.temperature = data[0];
            //         current_data.pressure = data[1];
            //     }
            //     _ => {
            //         println!(
            //             "Unknown data type: {}",
            //             received_text.split(":").collect::<Vec<&str>>()[0]
            //         );
            //     }
            // }

            // the data should be in the format of accel_x, accel_y, accel_z, angle_x, angle_y, angle_z, temp, pressure
            // the angle data is in the format of mdps, the acceleration data in the format of mg/LSB, the temperature in the format of degrees C, and the pressure in the format of hPa
            //the data is tellign us the change in angle, so we need to add the change to the current angle taking into account the time with the current timestamp
            print!("{}", data[0]);
            data[0] = data[0] / 1000.0;
            data[1] = data[1] / 1000.0;
            data[2] = data[2] / 1000.0;
            data[3] = data[3] / 1000.0;
            data[4] = data[4] / 1000.0;
            data[5] = data[5] / 1000.0;
            //if the absolute angle or acceleration is less than 0.01, we can assume it is 0
            let limit = 0.0;
            if data[0].abs() < limit {
                data[0] = 0.0;
            }
            if data[1].abs() < limit {
                data[1] = 0.0;
            }
            if data[2].abs() < limit {
                data[2] = 0.0;
            }
            if data[3].abs() < limit {
                data[3] = 0.0;
            }
            if data[4].abs() < limit {
                data[4] = 0.0;
            }
            if data[5].abs() < limit {
                data[5] = 0.0;
            }

            // acceleration z is facing down so should cancel out gravity
            // data[2] = data[2] / 16.384;
            // the data is in the format of degrees C, so we need to convert it to C
            data[6] = data[6];

            //update the current data with the new data, adding the changes to the current data and taking into account the time

            // println!("{:?}", data);
            current_data.accel_x = data[0] * (dt as f32);
            current_data.accel_y = data[1] * (dt as f32);
            current_data.accel_z = data[2] * (dt as f32);
            current_data.angle_x += data[3] * (dt as f32);
            current_data.angle_y += data[4] * (dt as f32);
            current_data.angle_z += data[5] * (dt as f32);
            current_data.vel_x = current_data.accel_x * dt;
            current_data.vel_y = current_data.accel_y * dt;
            current_data.vel_z = current_data.accel_z * dt;
            current_data.avg_accel = f32::sqrt(
                current_data.accel_x.powi(2) +
                    current_data.accel_y.powi(2) +
                    current_data.accel_z.powi(2)
            );
            current_data.avg_vel = f32::sqrt(
                current_data.vel_x.powi(2) + current_data.vel_y.powi(2) + current_data.vel_z.powi(2)
            );
            current_data.pos_x += current_data.vel_x * dt;
            current_data.pos_y += current_data.vel_y * dt;
            current_data.pos_z += current_data.vel_z * dt;
            current_data.temperature = data[6];
            current_data.pressure = data[7];
            current_data.time = chrono::Utc::now().timestamp_micros();

            // println!("{:?}", current_data);

            //convert it to a json object
            let data = json!(current_data);
            // println!("{:?}", data);
            // send data to server websocket
            ws_stream.send(Message::text(data.to_string())).await.unwrap();
        }
    } else {
        println!("Found {} ports:", ports.len());
        for p in ports {
            println!("  {}", p.port_name);
        }
    }
}
