import serial
import threading
import queue
import re


class SerialPublisher:
    def __init__(self):
        self.data_queue = queue.Queue()
        self.subscribers = [] 
        self.ser = serial.Serial()

        self.read_thread = threading.Thread(target=self.read_port)
        self.read_thread.daemon = True

        self.find_thread = threading.Thread(target=self.find_esp)
        self.find_thread.daemon = True

    def save_to_queue(self, data):
        self.data_queue.put(data)
        self.notify_subscribers(data)

    def notify_subscribers(self, data):
        i=0
        for callback in self.subscribers:
            i=i+1
            print(f"notifico {i}")
            callback(data)

    def subscribe(self, callback):
        self.subscribers.append(callback)
        print(f"serial_handler.py: Se suscribio alguien! Cantidad de suscriptores: {len(self.subscribers)}")
    
    def read_port(self):
            while True:
                try:
                    if self.ser.in_waiting > 0:
                        data = self.ser.readline().decode('utf-8').strip()
                        if data:
                            self.save_to_queue(data)  
                except:
                    if not self.ser.is_open:
                        self.notify_subscribers("#Z1!")
                        break                     

    def start_read_port(self):       
        self.read_thread.start()
    
    def find_esp(self):
        ports = list(serial.tools.list_ports.comports())
        for port in ports:
            print(f"Trying {port.device}...")
            try:
                self.ser.baudrate = 115200
                self.ser.port = port.device
                self.ser.timeout = 10000
                self.ser.open()

                print(f"{port.device}: INIT")
                self.ser.write(b"INIT")
                response = self.ser.readline().decode('utf-8').strip()

                if response == "ESP":
                    print(f"Connected to ESP on {port.device}")                   
                    self.start_read_thread()
                    break
                else:
                    print(f"Failed {port.device}")
                self.ser.close()
            except (OSError, serial.SerialException) as e:
                print(f"Failed to connect to {port.device}: {e}")

        print("Termina find_esp")
    
    def start_read_thread(self):
        self.read_thread = threading.Thread(target=self.read_port)
        self.read_thread.daemon = True
        self.read_thread.start()

    def stop_read_thread(self):
        self.ser.close()

    def start_find_thread(self):
        self.find_thread = threading.Thread(target=self.find_esp)
        self.find_thread.daemon = True
        self.find_thread.start()

    def send_data(self, data):
        self.ser.write(data)

publisher = SerialPublisher()
state_fbr = { "state": "disconnected" }