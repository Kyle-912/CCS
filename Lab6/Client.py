import socket
from random import randint
from time import sleep
import pickle
import struct
import serial
import socket

from struct import *


if __name__=="__main__":
    # Specify IP Host and Port
    HOST = '192.168.7.1'
    PORT = 12345

    # UART port and serial communication
    UART_PORT = "/dev/ttyO4" # UART4 device on BeagleBone
    BAUD_RATE = 115200
    ser = serial.Serial(UART_PORT, BAUD_RATE)

    # Connect to socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((HOST, PORT))
        print("Connected to server")

        # Receive message from server
        def receive_data(conn):
            raw_msglen = conn.recv(4)  # Receive message length
            if not raw_msglen:
                return None
            msglen = struct.unpack('>I', raw_msglen)[0]
            data = conn.recv(msglen)  # Receive message data
            return pickle.loads(data)

        message = receive_data(client_socket)
        print("Message received from server:", message)


    while True:
        pass

        # Create random set of values for packaging
        x = randint(0, 230)  # Adjust to display resolution (e.g., 240x240)
        y = randint(0, 270)

        # Limit width and height to ensure the rectangle fits on the screen
        max_width = 240 - x  # Remaining width on the screen
        max_height = 280 - y  # Remaining height on the screen
        width = randint(10, min(30, max_width))  # Width between 10 and remaining width
        height = randint(10, min(30, max_height))  # Height between 10 and remaining height

        blue = randint(0, 31) << 11  # Blue (5 bits in MSB)
        green = randint(0, 63) << 5  # Green (6 bits in the middle)
        red = randint(0, 31)         # Red (5 bits in LSB)
        color = (blue | green | red)  & 0xFFFF

        # Package and send data
        rectangle_data = struct.pack('HHHHH', x, y, width, height, color)
        ser.write(rectangle_data)  # Send serialized data over UART
        print(f"Rectangle data sent over UART: x={x}, y={y}, width={width}, height={height}, color={hex(color)}")

        # Add delay if needed
        sleep(1)
