import serial
import threading
import queue
import time


class SerialPublisher:
    def __init__(self):
        self.data_queue = queue.Queue()
        self.subscribers = [] 
        self.ser = serial.Serial()
        self.start_time = 0

        self.read_thread = threading.Thread(target=self.read_port)
        self.read_thread.daemon = True

        self.find_thread = threading.Thread(target=self.find_esp)
        self.find_thread.daemon = True

    def save_to_queue(self, data):
        #self.data_queue.put(data)
        start_time = time.time()
        self.notify_subscribers(data)
        end_time = time.time()
        print(f"Tiempo que tarda notify_subscribers {end_time - start_time:.2f} seconds")

    def notify_subscribers(self, data):
        i=0
        for callback in self.subscribers:
            i=i+1
            #print(f"Notifico al callback:", callback.__name__)
            callback(data)

    def subscribe(self, callback):
        self.subscribers.append(callback)
        print(f"serial_handler.py: Se suscribio alguien! Cantidad de suscriptores: {len(self.subscribers)}")
        
    def unsubscribe(self, callback):
        if callback in self.subscribers:
            self.subscribers.remove(callback)
            print(f"serial_handler.py: Alguien se desuscribió. Cantidad de suscriptores: {len(self.subscribers)}")
    
    def read_port(self):
        buffer = ""
        last_received_time = time.time()
        print("Starting read_port")

        while True:
            try:
                if self.ser.in_waiting > 0:
                    char = self.ser.read(1).decode('utf-8')
                    buffer += char
                    last_received_time = time.time()

                    if char == '\n':
                        end_time = time.time()  # End timing the transfer
                        print(f"Tiempo en que llega una linea completa {end_time - self.start_time:.2f} seconds")
                        #if buffer.strip().startswith("I:"):
                        #    print(buffer.strip())
                        #else:
                            #if not (buffer.strip() == "#OK!"):
                        
                        print(f"----------------- ESP Response: {buffer.strip()} -----------------")
                        self.save_to_queue(buffer.strip())
                        buffer = ""
                        self.start_time = time.time()
                else:
                    if time.time() - last_received_time > 5:    # Si el fin de la cadena no llega en 5 segundos, se limpia el buffer
                        if buffer:
                            print(f"Timeout reached. Flushing buffer: {buffer.strip()}")
                            buffer = ""
                        last_received_time = time.time()
            except Exception as e:
                print(f"Exception occurred: {e}")
                if not self.ser.is_open:
                    print("Serial port is closed. Notifying subscribers and breaking loop.")
                    self.notify_subscribers("#Z1!")
                    break        

    def start_read_port(self):       
        self.read_thread.start()
    
    def find_esp(self):
        ports = list(serial.tools.list_ports.comports())
        for port in ports:
            print(f"\nTrying {port.device}...")
            try:
                self.ser.baudrate = 230400
                self.ser.port = port.device
                self.ser.timeout = 2  # Reduced timeout for better responsiveness
                self.ser.open()
                
                # Wait for initial bootup messages
                print(f"Reading initial logs from {port.device}...")
                initial_wait = 1.0  # Wait 1 second for bootup messages
                time.sleep(initial_wait)
                
                # Read and print all available initial messages
                while self.ser.in_waiting > 0:
                    try:
                        buffer = self.ser.read(self.ser.in_waiting).decode('utf-8')
                        if buffer.strip():  # Only print non-empty messages
                            print(f"Initial logs: {buffer.strip()}")
                        time.sleep(0.1)  # Small delay to allow more data to arrive
                    except UnicodeDecodeError:
                        print("Warning: Received non-UTF8 data, skipping...")
                        self.ser.flush()
                        break
                
                # Now send the INIT command and wait for response
                print(f"\n{port.device}: Sending #INIT!")
                self.ser.write("#INIT!\n".encode())
                
                # Wait for response with timeout
                start_time = time.time()
                response = ""
                timeout = 5  # 5 seconds timeout for response
                
                while time.time() - start_time < timeout:
                    if self.ser.in_waiting > 0:
                        response = self.ser.readline().decode('utf-8').strip()
                        print("Response:", response)
                        if response == "#ESP!":
                            print(f"Successfully connected to ESP on {port.device}")
                            self.start_read_thread()
                            return True
                        time.sleep(0.1)
                
                print(f"No valid response from {port.device}")
                self.ser.close()
                
            except (OSError, serial.SerialException) as e:
                print(f"Failed to connect to {port.device}: {e}")
                if self.ser.is_open:
                    self.ser.close()
                continue
        
        print("No ESP32 device found")
    
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
        print("Serial Publisher (send_data)", data)
        self.ser.write(data)

publisher = SerialPublisher()
state_fbr = { "state": "disconnected" }
cycle_id = ""

id_list = []
ph_list = []
od_list = []
temp_list = []
light_list = []