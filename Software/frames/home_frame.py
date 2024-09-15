import customtkinter as ctk
from PIL import Image
import os

class InstantValuesFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 

        image_path = os.path.join(os.getcwd(), "images") 
        small_font = ctk.CTkFont(size=12, weight="bold")
        big_font = ctk.CTkFont(size=16, weight="bold")

        # set grid layout 1x3
        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(2, weight=1)

        # Left 
        self.co2_label = ctk.CTkLabel(self, text="CO2", anchor="w", font=small_font)
        self.co2_label.grid(row=0, column=0, padx=20, pady=(10, 0))
        self.co2_button = ctk.CTkButton(self, text="Encendido", fg_color="green", command=self.co2_button_event, hover=False)
        self.co2_button.grid(row=1, column=0, padx=20, pady=(10, 0))

        self.o2_label = ctk.CTkLabel(self, text="O2", anchor="w", font=small_font)
        self.o2_label.grid(row=2, column=0, padx=20, pady=(10, 0))
        self.o2_button = ctk.CTkButton(self, text="Encendido", fg_color="green", command=self.o2_button_event, hover=False)
        self.o2_button.grid(row=3, column=0, padx=20, pady=(10, 0))

        self.air_label = ctk.CTkLabel(self, text="Aire", anchor="w", font=small_font)
        self.air_label.grid(row=4, column=0, padx=20, pady=(10, 0))
        self.air_button = ctk.CTkButton(self, text="Encendido", fg_color="green", command=self.air_button_event, hover=False)
        self.air_button.grid(row=5, column=0, padx=20, pady=(10, 0))

        self.pump_label = ctk.CTkLabel(self, text="Bomba", anchor="w", font=small_font)
        self.pump_label.grid(row=6, column=0, padx=20, pady=(10, 0))
        self.pump_button = ctk.CTkButton(self, text="Encendido", fg_color="green", command=self.pump_button_event, hover=False)
        self.pump_button.grid(row=7, column=0, padx=20, pady=(10, 0))

        # Center
        # self.reactor_image_frame = ctk.CTkFrame(master=self)
        # self.reactor_image_frame.grid(row=0, column=1, sticky="nsew")
        # self.reactor_image = ctk.CTkImage(Image.open(os.path.join(image_path, "reactor.png")))
        # bg_image_label = ctk.CTkLabel(master=self.reactor_image_frame, image=self.reactor_image)
        # bg_image_label.pack(side="top", fill="both", expand=True)
        

        # Right 
        self.light_label = ctk.CTkLabel(self, text="Luz", anchor="w", font=small_font)
        self.light_label.grid(row=0, column=2, padx=20, pady=10)
        self.light_label_info = ctk.CTkLabel(self, text="50%", anchor="w", fg_color="white", font=small_font, padx=20, pady=15, corner_radius=5)
        self.light_label_info.grid(row=1, column=2, padx=20, pady=10)

        self.pH_label = ctk.CTkLabel(self, text="pH", anchor="w", font=small_font)
        self.pH_label.grid(row=2, column=2, padx=20, pady=10)
        self.pH_label_info = ctk.CTkLabel(self, text="7.032", anchor="w", fg_color="white", font=small_font, padx=20, pady=15, corner_radius=5)
        self.pH_label_info.grid(row=3, column=2, padx=20, pady=10)

        self.do_label = ctk.CTkLabel(self, text="DO", anchor="w", font=small_font)
        self.do_label.grid(row=4, column=2, padx=20, pady=10)
        self.do_label_info = ctk.CTkLabel(self, text="75.3%", anchor="w", fg_color="white", font=small_font, padx=20, pady=15, corner_radius=5)
        self.do_label_info.grid(row=5, column=2, padx=20, pady=10)

        self.temp_label = ctk.CTkLabel(self, text="Temperatura", anchor="w", font=small_font)
        self.temp_label.grid(row=6, column=2, padx=20, pady=10)
        self.temp_label_info = ctk.CTkLabel(self, text="24.3°C", anchor="w", fg_color="white", font=small_font, padx=20, pady=15, corner_radius=5)
        self.temp_label_info.grid(row=7, column=2, padx=20, pady=10)

    def co2_button_event(self):
        if self.co2_button.cget("text") == "Encendido":
            self.co2_button.configure(text="Apagado")
            self.co2_button.configure(fg_color="red")
        elif self.co2_button.cget("text") == "Apagado":
            self.co2_button.configure(text="Encendido")
            self.co2_button.configure(fg_color="green")

    def o2_button_event(self):
        if self.o2_button.cget("text") == "Encendido":
            self.o2_button.configure(text="Apagado")
            self.o2_button.configure(fg_color="red")
        elif self.o2_button.cget("text") == "Apagado":
            self.o2_button.configure(text="Encendido")
            self.o2_button.configure(fg_color="green")
    
    def air_button_event(self):
        if self.air_button.cget("text") == "Encendido":
            self.air_button.configure(text="Apagado")
            self.air_button.configure(fg_color="red")
        elif self.air_button.cget("text") == "Apagado":
            self.air_button.configure(text="Encendido")
            self.air_button.configure(fg_color="green")

    def pump_button_event(self):
        if self.pump_button.cget("text") == "Encendido":
            self.pump_button.configure(text="Apagado")
            self.pump_button.configure(fg_color="red")
        elif self.pump_button.cget("text") == "Apagado":
            self.pump_button.configure(text="Encendido")
            self.pump_button.configure(fg_color="green")

class HomeFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master, corner_radius=0, fg_color="transparent")    
        
        top_frame = ctk.CTkFrame(master=self)
        top_frame.pack(side="top", fill="both", expand=True)

        bottom_frame = ctk.CTkFrame(master=self)
        bottom_frame.pack(side="top", fill="both", expand=True)

        instant_values_frame = InstantValuesFrame(top_frame)
        instant_values_frame.pack(side="left", fill="both", expand=True)

        home_frame_1_2 = ctk.CTkFrame(master=top_frame, fg_color="orange")
        home_frame_1_2.pack(side="left", fill="both", expand=True)

        home_frame_2_1 = ctk.CTkFrame(master=bottom_frame, fg_color="green")
        home_frame_2_1.pack(side="left", fill="both", expand=True)

        home_frame_2_2 = ctk.CTkFrame(master=bottom_frame, fg_color="purple")
        home_frame_2_2.pack(side="left", fill="both", expand=True)

