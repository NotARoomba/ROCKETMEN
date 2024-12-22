import serial
import time

def main():
    # Configuration
    port = "COM6"  # Specify the COM port
    baudrate = 9600  # Specify the baud rate
    timeout = 1  # Timeout in seconds
    
    try:
        # Open the serial port
        with serial.Serial(port, baudrate, timeout=timeout) as ser:
            print(f"Opened {port} successfully.")

            # Clear input and output buffers to avoid residual data
            ser.reset_input_buffer()
            ser.reset_output_buffer()

            # Send 0x7F
            command = bytes([0x7F])
            ser.write(command)
            print(f"Sent: 0x7F")

            # Wait for response
            time.sleep(0.1)  # Optional small delay before reading
            response = ser.read(100)  # Read up to 100 bytes
            if response:
                print(f"Received: {response.hex()}")
            else:
                print("No response received.")
    
    except serial.SerialException as e:
        print(f"Error opening or using serial port: {e}")
        ser.close()
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    main()
