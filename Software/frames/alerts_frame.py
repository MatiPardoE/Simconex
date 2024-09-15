import customtkinter as ctk

class AlertsFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master, corner_radius=0, fg_color="transparent")
        
        self.alerts_frame_label = ctk.CTkLabel(self, text="Alerts Frame")
        self.alerts_frame_label.grid(row=0, column=0, padx=20, pady=10)
