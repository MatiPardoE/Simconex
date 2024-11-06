import time
from enum import Enum
import frames.serial_handler as ui_serial
from frames.serial_handler import data_lists
import re
import csv
import os

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

        # Listas temporales acumuladoras de toda la comunicacion
        self.id_list = []
        self.ph_list = []
        self.od_list = []
        self.temp_list = []
        self.light_list = []
        self.co2_list = []
        self.o2_list = []
        self.n2_list = []
        self.air_list = []

        # Listas temporales acumuladoras de cada bloque
        self.id_list_tmp = []
        self.ph_list_tmp = []
        self.od_list_tmp = []
        self.temp_list_tmp = []
        self.light_list_tmp = []
        self.co2_list_tmp = []
        self.o2_list_tmp = []
        self.n2_list_tmp = []
        self.air_list_tmp = []

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

            self.generate_cycleout_file()
            ui_serial.publisher.notify_sync()           
        
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
        pattern = r"^(\d{8}),(\d{2}\.\d{2}),(\d{3}\.\d{2}),(\d{2}\.\d{2}),(\d{2}),(0|1),(0|1),(0|1),(0|1)$"
        if data == "#DATAOUT1!":
            self.id_list += self.id_list_tmp
            self.light_list += self.light_list_tmp
            self.ph_list += self.ph_list_tmp
            self.od_list += self.od_list_tmp
            self.temp_list += self.temp_list_tmp
            self.co2_list += self.co2_list_tmp
            self.o2_list += self.o2_list_tmp
            self.n2_list += self.n2_list_tmp
            self.air_list += self.air_list_tmp
        
            data_lists['id'] = self.id_list
            data_lists['light'] = self.light_list
            data_lists['ph'] = self.ph_list
            data_lists['od'] = self.od_list
            data_lists['temperature'] = self.temp_list
            data_lists['co2'] = self.co2_list
            data_lists['o2'] = self.o2_list
            data_lists['n2'] = self.n2_list
            data_lists['air'] = self.air_list
            print("Valid block of data received")
            ui_serial.publisher.send_data(b"#OK!\n")
            self.handshake_status = HandshakeStatus.DATAOUT1
            return
        match = re.match(pattern, data)
        if match: 
            self.line_count += 1
            self.id_list_tmp.append(int(match.group(1)))
            self.light_list_tmp.append(int(match.group(5)))
            self.ph_list_tmp.append(float(match.group(2)))
            self.od_list_tmp.append(float(match.group(3)))
            self.temp_list_tmp.append(float(match.group(4)))
            self.co2_list_tmp.append(int(match.group(6)))
            self.o2_list_tmp.append(int(match.group(7)))
            self.n2_list_tmp.append(int(match.group(8)))
            self.air_list_tmp.append(int(match.group(9)))
            self.handshake_status = HandshakeStatus.MSG_VALID

            if self.line_count == 160:
                print("Valid block of data received")
                self.line_count = 0

                self.id_list += self.id_list_tmp
                self.light_list += self.light_list_tmp
                self.ph_list += self.ph_list_tmp
                self.od_list += self.od_list_tmp
                self.temp_list += self.temp_list_tmp
                self.co2_list += self.co2_list_tmp
                self.o2_list += self.o2_list_tmp
                self.n2_list += self.n2_list_tmp
                self.air_list += self.air_list_tmp

                self.id_list_tmp = []
                self.ph_list_tmp = []
                self.od_list_tmp = []
                self.temp_list_tmp = []
                self.light_list_tmp = []
                self.co2_list_tmp = []
                self.o2_list_tmp = []
                self.n2_list_tmp = []
                self.air_list_tmp = []
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
            self.co2_list_tmp = []
            self.o2_list_tmp = []
            self.n2_list_tmp = []
            self.air_list_tmp = []

            ui_serial.publisher.send_data("#FAIL!\n")
    
    def wait_for_response(self, data):
        pattern = r'\b\d{8}_\d{4}\b'        

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
        
        print("") # NO SACAR ESTE PRINT QUE SE ROMPE LA COMUNICACION

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
    
    def generate_cycleout_file(self):
        fname = os.path.join(os.getcwd(), "Log", ui_serial.cycle_id, "cycle_out_"+ui_serial.cycle_id+".csv")
        with open(fname, "w", newline='') as csvfile:
            writer = csv.writer(csvfile)
            
            for i in range(len(data_lists["id"])):
                row = [
                    f"{data_lists['id'][i]:08d}",      
                    f"{data_lists['ph'][i]:05.2f}",       
                    f"{data_lists['od'][i]:06.2f}",     
                    f"{data_lists['temperature'][i]:05.2f}",  
                    f"{data_lists['light'][i]:02d}",
                    data_lists['co2'][i],
                    data_lists['o2'][i],
                    data_lists['n2'][i],
                    data_lists['air'][i] 
                ]
                writer.writerow(row)
