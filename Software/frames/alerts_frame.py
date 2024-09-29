import customtkinter as ctk
import serial

class LogFrame(ctk.CTkScrollableFrame):

    def __init__(self, master):
        super().__init__(master) 

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=1)

        self.label_control = ctk.CTkLabel(self, text="Registro de alertas", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=(20, 10), pady=(10, 0), sticky="w")

        datalog = [
            ["Hora ", "  Fecha   ", "Descripcion", "Prioridad"],
            ["12:00", "21/09/2024", "El sensor de pH requiere calibración", "Alta"],
            ["12:30", "21/09/2024", "Pérdida de energía", "Alta"],
            ["13:00", "21/09/2024", "OD fuera de rango", "Media"],
            ["13:30", "21/09/2024", "El sensor de OD requiere calibración", "Alta"],
            ["14:00", "21/09/2024", "pH fuera de rango", "Media"]
        ]

        self.frame_lines = ctk.CTkFrame(self, width=1500)
        self.frame_lines.grid(row=1, column=0, padx=10, pady=10)
        self.frame_lines.grid_columnconfigure(0, weight=1)
        self.frame_lines.grid_rowconfigure(0, weight=1)

        for i in range(len(datalog)):
            self.frame_line = ctk.CTkFrame(self.frame_lines)
            self.frame_line.grid(row=i, column=0, padx=5, pady=0, sticky="ew")

            self.frame_line.grid_rowconfigure(0, weight=1)
            self.frame_line.grid_columnconfigure(0, weight=1)
            self.frame_line.grid_columnconfigure(1, weight=1)
            self.frame_line.grid_columnconfigure(2, weight=1)
            self.frame_line.grid_columnconfigure(3, weight=1)

            self.label_time = ctk.CTkLabel(self.frame_line, text=datalog[i][0], corner_radius=0, width=100)
            self.label_time.grid(row=0, column=0, padx=5, pady=0, sticky="ns")

            self.label_date = ctk.CTkLabel(self.frame_line, text=datalog[i][1], corner_radius=0, width=150)
            self.label_date.grid(row=0, column=1, padx=5, pady=0, sticky="ns")

            self.label_description = ctk.CTkLabel(self.frame_line, text=datalog[i][2], corner_radius=0, width=500)
            self.label_description.grid(row=0, column=2, padx=5, pady=0, sticky="ns")

            self.label_priority = ctk.CTkLabel(self.frame_line, text=datalog[i][3], corner_radius=0, width=100)
            self.label_priority.grid(row=0, column=3, padx=5, pady=0, sticky="ns")
            
            if datalog[i][3] == "Alta":
                self.label_priority.configure(font=ctk.CTkFont(weight="bold"))

            if i==0:
                self.label_time.configure(font=ctk.CTkFont(size=15, weight="bold"))
                self.label_date.configure(font=ctk.CTkFont(size=15, weight="bold"))
                self.label_description.configure(font=ctk.CTkFont(size=15, weight="bold"))
                self.label_priority.configure(font=ctk.CTkFont(size=15, weight="bold"))

class AlertsFrame(ctk.CTkFrame):
    def __init__(self, master, ser):
        super().__init__(master, corner_radius=0, fg_color="transparent")

        self.serial = ser

        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(0, weight=1) 
        
        self.log_frame = LogFrame(self)
        self.log_frame.grid(row=0, column=0, padx=10, pady=(10, 0), sticky="nsew")

    def update_serial_obj(self, ser):
        self.serial = ser