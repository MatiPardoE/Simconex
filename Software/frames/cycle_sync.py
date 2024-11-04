import time
from enum import Enum
import frames.serial_handler as ui_serial
import re

class HandshakeStatus(Enum):
    MSG_VALID = 7
    TIMESTAMP = 6
    DATAOUT1 = 5
    DATAOUT0 = 4
    ID1 = 3
    ID0 = 2
    OK = 1
    NOT_YET = 0
    TIMEOUT = -1
    ERROR = -2
    DATA_FAIL = -3

class CycleSync:
    def __init__(self):
        self.handshake_status = HandshakeStatus.NOT_YET
        self.start_validate_time = 0

        self.id_list = []
        self.ph_list = []
        self.od_list = []
        self.temp_list = []
        self.light_list = []

        self.id_list_tmp = []
        self.ph_list_tmp = []
        self.od_list_tmp = []
        self.temp_list_tmp = []
        self.light_list_tmp = []

        self.line_count = 0

    def sync_running_cycle(self, timeout=3):
        print("Syncronization of running cycle started!")

        try:         
            ui_serial.publisher.subscribe(self.wait_for_response)
            self.send_data_and_wait_hs(b"#SYNC0!\n")
            self.wait_message(HandshakeStatus.ID0, timeout)
            ui_serial.publisher.send_data(b"#OK!\n")
            self.wait_message(HandshakeStatus.TIMESTAMP, timeout)
            ui_serial.publisher.send_data(b"#OK!\n")
            self.wait_message(HandshakeStatus.ID1, timeout)
            ui_serial.publisher.send_data(b"#OK!\n")
            self.wait_message(HandshakeStatus.DATAOUT0, timeout)
            ui_serial.publisher.send_data(b"#OK!\n")
            ui_serial.publisher.unsubscribe(self.wait_for_response)

            start_time = time.time()  # Start timing the transfer

            ui_serial.publisher.subscribe(self.wait_for_dataout)
            self.receive_data(timeout=5)
            ui_serial.publisher.unsubscribe(self.wait_for_dataout)

            end_time = time.time()  # End timing the transfer
            print(f"Transfer completed in {end_time - start_time:.2f} seconds")

            ui_serial.publisher.subscribe(self.wait_for_response)
            self.send_data_and_wait_hs(b"#SYNC1!\n")
            ui_serial.publisher.unsubscribe(self.wait_for_response) 

            ui_serial.state_fbr["state"] = "running"           
        
        except Exception as e:
            print("Syncronization of running cycle failed!")
            print(e)
            ui_serial.publisher.unsubscribe(self.wait_for_response)
            ui_serial.publisher.unsubscribe(self.wait_for_dataout)

    def wait_for_ok(self, data):
        if data.strip() == "#OK!":
            self.handshake_status = HandshakeStatus.OK
        elif data.strip() == "#FAIL!":
            self.handshake_status = HandshakeStatus.DATA_FAIL
        else:
            print("Received unexpected data in HS:", data.strip())
            self.handshake_status = HandshakeStatus.ERROR
    
    def receive_data(self, timeout=5):
        start_time = time.time()
        while True:
            if self.handshake_status == HandshakeStatus.DATAOUT1:
                print("Handshake OK")
                start_time = time.time()
                self.handshake_status = HandshakeStatus.NOT_YET
                break
            if self.handshake_status == HandshakeStatus.MSG_VALID:
                print("Valid line of data received")
                start_time = time.time()
                self.handshake_status = HandshakeStatus.NOT_YET
            if self.handshake_status == HandshakeStatus.DATA_FAIL:
                raise ValueError("Data lost between ESP and UI")
            if time.time() - start_time > timeout:
                self.handshake_status = HandshakeStatus.TIMEOUT
                raise TimeoutError("Timeout waiting for handshake from ESP")

    def wait_for_dataout(self, data):
        pattern = r"^(\d{8}),(\d{2}\.\d{2}),(\d{3}\.\d{2}),(\d{2}\.\d{2}),(\d{2})$"
        if data == "#DATAOUT1!":
            ui_serial.id_list = self.id_list
            ui_serial.light_list = self.light_list
            ui_serial.ph_list = self.ph_list
            ui_serial.od_list = self.od_list
            ui_serial.temp_list = self.temp_list
            print("Valid block of data received")
            ui_serial.publisher.send_data(b"#OK!\n")
            self.handshake_status = HandshakeStatus.DATAOUT1
            return
        match = re.match(pattern, data)
        if match: 
            self.line_count += 1
            self.id_list_tmp.insert(0, int(match.group(1)))
            self.light_list_tmp.insert(0, int(match.group(5)))
            self.ph_list_tmp.insert(0, float(match.group(2)))
            self.od_list_tmp.insert(0, float(match.group(3)))
            self.temp_list_tmp.insert(0, float(match.group(4)))
            self.handshake_status = HandshakeStatus.MSG_VALID

            if self.line_count == 160:
                print("Valid block of data received")
                self.line_count = 0

                self.id_list += self.id_list_tmp
                self.light_list += self.light_list_tmp
                self.ph_list += self.ph_list_tmp
                self.od_list += self.od_list_tmp
                self.temp_list += self.temp_list_tmp

                self.id_list_tmp = []
                self.ph_list_tmp = []
                self.od_list_tmp = []
                self.temp_list_tmp = []
                self.light_list_tmp = []
                ui_serial.publisher.send_data(b"#OK!\n")
        else: 
            self.line_count = 0                   
            print("Mensaje desconocido:", data)
            time.sleep(0.3)   
            self.id_list_tmp = []
            self.ph_list_tmp = []
            self.od_list_tmp = []
            self.temp_list_tmp = []
            self.light_list_tmp = []

            ui_serial.publisher.send_data("#FAIL!\n")
    
    def wait_for_response(self, data):
        pattern = r'\b\d{8}_\d{4}\b'
        print("wait for response data:", data)

        if data.strip() == "#OK!":
            self.handshake_status = HandshakeStatus.OK
        elif data.strip() == "#FAIL!":
            self.handshake_status = HandshakeStatus.DATA_FAIL
        elif data.strip() == "#ID0!":
            self.handshake_status = HandshakeStatus.ID0
        elif data.strip() == "#ID1!":
            self.handshake_status = HandshakeStatus.ID1
        elif data.strip() == "#DATAOUT0!":
            self.handshake_status = HandshakeStatus.DATAOUT0
        elif data.strip() == "#DATAOUT1!":
            self.handshake_status = HandshakeStatus.DATAOUT1
        else:
            if re.match(pattern, data):
                ui_serial.cycle_id = data
                self.handshake_status = HandshakeStatus.TIMESTAMP
            else:
                print("Received unexpected data in HS:", data.strip())
                self.handshake_status = HandshakeStatus.ERROR

    def send_data_and_wait_hs(self, data, timeout = 3):
        self.handshake_status = HandshakeStatus.NOT_YET
        ui_serial.publisher.send_data(data)
        self.wait_handshake(timeout)
    
    def wait_handshake(self,timeout = 5):
        start_time = time.time()
        while True:
            if self.handshake_status == HandshakeStatus.OK:
                print("Handshake OK")
                self.handshake_status = HandshakeStatus.NOT_YET
                break
            if self.handshake_status == HandshakeStatus.DATA_FAIL:
                raise ValueError("Data lost between ESP and UI")
            if time.time() - start_time > timeout:
                self.handshake_status = HandshakeStatus.TIMEOUT
                raise TimeoutError("Timeout waiting for handshake from ESP")
    
    def wait_message(self, status, timeout = 5): # Implementar esto en todos los handshakes porque es mas generico
        start_time = time.time()
        while True:
            if self.handshake_status == status:
                print("Recibi la respuesta esperada")
                self.handshake_status = HandshakeStatus.NOT_YET
                break
            if self.handshake_status == HandshakeStatus.DATA_FAIL:
                raise ValueError("Data lost between ESP and UI")
            if time.time() - start_time > timeout:
                self.handshake_status = HandshakeStatus.TIMEOUT
                raise TimeoutError("Timeout waiting for handshake from ESP")
