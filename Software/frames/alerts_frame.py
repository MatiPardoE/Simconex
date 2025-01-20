import customtkinter as ctk
import frames.serial_handler as ui_serial
from frames.serial_handler import MsgType

class LogFrame(ctk.CTkScrollableFrame):

    def __init__(self, master):
        super().__init__(master) 

        self.alert_list = []
        ui_serial.publisher.subscribe(self.update_log) 

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=1)

        self.label_control = ctk.CTkLabel(self, text="Registro de alertas", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=(20, 10), pady=(10, 0), sticky="w")

        self.frame_lines = ctk.CTkFrame(self, width=1500)
        self.frame_lines.grid(row=1, column=0, padx=10, pady=10)
        self.frame_lines.grid_columnconfigure(0, weight=1)
        self.frame_lines.grid_rowconfigure(0, weight=1)

        self.frame_line = ctk.CTkFrame(self.frame_lines)
        self.frame_line.grid(row=0, column=0, padx=0, pady=0, sticky="ew")

        self.frame_line.grid_rowconfigure(0, weight=1)
        self.frame_line.grid_columnconfigure(0, weight=1)
        self.frame_line.grid_columnconfigure(1, weight=1)
        self.frame_line.grid_columnconfigure(2, weight=1)
        self.frame_line.grid_columnconfigure(3, weight=1)

        self.label_time = ctk.CTkLabel(self.frame_line, text="Hora", corner_radius=0, width=100, font=ctk.CTkFont(size=15, weight="bold"))
        self.label_time.grid(row=0, column=0, padx=5, pady=0, sticky="ns")

        self.label_date = ctk.CTkLabel(self.frame_line, text="Fecha", corner_radius=0, width=150, font=ctk.CTkFont(size=15, weight="bold"))
        self.label_date.grid(row=0, column=1, padx=5, pady=0, sticky="ns")

        self.label_description = ctk.CTkLabel(self.frame_line, text="Descripcion", corner_radius=0, width=500, font=ctk.CTkFont(size=15, weight="bold"))
        self.label_description.grid(row=0, column=2, padx=5, pady=0, sticky="ns")

        self.label_priority = ctk.CTkLabel(self.frame_line, text="Prioridad", corner_radius=0, width=100, font=ctk.CTkFont(size=15, weight="bold"))
        self.label_priority.grid(row=0, column=3, padx=5, pady=0, sticky="ns")

        self.frame_log = ctk.CTkFrame(self.frame_lines)
        self.frame_log.grid(row=1, column=0, padx=5, pady=0, sticky="ew")

        self.scrollable_frame = ctk.CTkScrollableFrame(self.frame_log, width=1500)
        self.scrollable_frame.pack(pady=10, padx=10, fill="both", expand=True)
    
    def update_log(self, data):
        if data == MsgType.NEW_CYCLE_SENT or data == MsgType.ESP_DISCONNECTED or data == MsgType.CYCLE_DELETED:
            for widget in self.scrollable_frame.winfo_children():
                widget.destroy()
            return 
        
        if data == MsgType.PH_OUT_OF_RANGE or data == MsgType.OD_OUT_OF_RANGE or data == MsgType.TEMP_OUT_OF_RANGE:
            self.frame_line = ctk.CTkFrame(self.scrollable_frame)
            self.frame_line.pack(fill="x")

            self.in_frame = ctk.CTkFrame(self.frame_line)
            self.in_frame.pack(fill="x")
            
            self.label_time = ctk.CTkLabel(self.in_frame, text="hour", corner_radius=0, width=150) 
            self.label_time.pack(side='left')

            self.label_date = ctk.CTkLabel(self.in_frame, text="date", corner_radius=0, width=200) 
            self.label_date.pack(side='left')

            if data == MsgType.PH_OUT_OF_RANGE:
                self.label_description = ctk.CTkLabel(self.in_frame, text="pH fuera de rango", corner_radius=0, width=150)
            elif data == MsgType.OD_OUT_OF_RANGE:
                self.label_description = ctk.CTkLabel(self.in_frame, text="OD fuera de rango", corner_radius=0, width=150)
            elif data == MsgType.TEMP_OUT_OF_RANGE:
                self.label_description = ctk.CTkLabel(self.in_frame, text="Temperatura fuera de rango", corner_radius=0, width=150)
            
            self.label_description.pack(side='left')

            self.label_priority = ctk.CTkLabel(self.in_frame, text="Alta", corner_radius=0, width=150)
            self.label_priority.pack(side='left')

            self.scrollable_frame._parent_canvas.yview_moveto(1.0)

class AlertsFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master, corner_radius=0, fg_color="transparent")

        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(0, weight=1) 
        
        self.log_frame = LogFrame(self)
        self.log_frame.grid(row=0, column=0, padx=10, pady=(10, 0), sticky="nsew")