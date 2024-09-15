import customtkinter as ctk

class CalibrationFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master, corner_radius=0, fg_color="transparent")
        
        self.calibration_frame_label = ctk.CTkLabel(self, text="Calibration Frame")
        self.calibration_frame_label.grid(row=0, column=0, padx=20, pady=10)
