import socket
import pickle
import struct

if __name__=="__main__":
    pass
    # Specify IP Host and Port
    HOST = '192.168.7.1'
    PORT = 12345

    # Connect to socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:

        # Specify timeout period
        server_socket.settimeout(30.0) # Timeout set to 30 seconds

        # Bind port and begin listening for a connection
        server_socket.bind((HOST, PORT))
        server_socket.listen(1)
        print(f"Server listening on {HOST}:{PORT}")

        conn, addr = server_socket.accept()
        with conn:
            print("Connected by", addr)

            # Send a random message to the client
            message = "Testing"  # Static message
            data = pickle.dumps(message)  # Serialize the message
            conn.sendall(struct.pack('>I', len(data)) + data)  # Prefix with 4-byte length
            print("Message sent to client:", message)

        # Close the server
        conn.close()
        print("Connection closed")
