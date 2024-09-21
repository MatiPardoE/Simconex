import customtkinter as ctk
from PIL import Image
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import os

class ActualCycleFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master) 

        self.grid_columnconfigure(0, weight=1)
        #self.grid_rowconfigure(4, weight=1)

        self.label_actual = ctk.CTkLabel(self, text="Ciclo Actual", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_actual.grid(row=0, column=0, padx=(20, 10), pady=(10, 0), sticky="w")

        self.label_actual_days = ctk.CTkLabel(self, text="10 Dias", font=ctk.CTkFont(size=18))
        self.label_actual_days.grid(row=1, column=0, padx=(20, 10), pady=(10, 10), sticky="w")

        self.progressbar_actual = ctk.CTkProgressBar(self)
        self.progressbar_actual.grid(row=2, column=0, padx=(20, 10), pady=(10, 10), sticky="ew")

        self.frame_actual = ctk.CTkFrame(self)
        self.frame_actual.grid(row=3, column=0, padx=(20, 10), pady=(10, 10), sticky="ew")

        self.frame_actual.grid_columnconfigure((0, 1), weight=1)
        self.frame_actual.grid_rowconfigure((0, 1), weight=1)

        self.label_done_colour = ctk.CTkLabel(self.frame_actual, text="Completo")
        self.label_done_colour.grid(row=0, column=0, padx=20, pady=0, sticky="w")

        self.label_left_colour = ctk.CTkLabel(self.frame_actual, text="Restante")
        self.label_left_colour.grid(row=0, column=1, padx=20, pady=0, sticky="w")

        self.label_done_text = ctk.CTkLabel(self.frame_actual, text="5 dias", font=ctk.CTkFont(size=15, weight="bold"))
        self.label_done_text.grid(row=1, column=0, padx=20, pady=0, sticky="w")

        self.label_left_text = ctk.CTkLabel(self.frame_actual, text="5 dias", font=ctk.CTkFont(size=15, weight="bold"))
        self.label_left_text.grid(row=1, column=1, padx=20, pady=0, sticky="w")

class ControlCycleFrame(ctk.CTkFrame):
    
    def __init__(self, master):
        super().__init__(master) 

        def callback(event):
            image_path = os.path.join(os.getcwd(), "images")
        #     play_image = ctk.CTkImage(Image.open(os.path.join(image_path, "play.png")), size=(40, 40))
        #     pause_image = ctk.CTkImage(Image.open(os.path.join(image_path, "pause.png")), size=(40, 40))

        #     tk_image = ctk.Ctkim(pause_image)

        #     #if event.widget.cget("image") == play_image:
        #     #event.widget.config(image=pause_image)
        #     #else:
        #     #    event.widget.config(image=play_image)

        image_path = os.path.join(os.getcwd(), "images")

        self.grid_columnconfigure(0, weight=1)

        self.label_control = ctk.CTkLabel(self, text="Control del ciclo", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=20, pady=(10, 0), sticky="w")

        self.frame_buttons = ctk.CTkFrame(self)
        self.frame_buttons.grid(row=1, column=0, padx=20, pady=(10, 0), sticky="ew")

        self.frame_buttons.grid_rowconfigure(0, weight=1)
        self.frame_buttons.grid_columnconfigure(0, weight=1)
        self.frame_buttons.grid_columnconfigure(1, weight=1)
        self.frame_buttons.grid_columnconfigure(2, weight=1)
        self.frame_buttons.grid_columnconfigure(3, weight=1)
        self.frame_buttons.grid_columnconfigure(4, weight=1)
        self.frame_buttons.grid_columnconfigure(5, weight=1)

        self.play_pause_image = ctk.CTkImage(Image.open(os.path.join(image_path, "play.png")), size=(40, 40))
        self.play_pause_image_label = ctk.CTkLabel(self.frame_buttons, text="", image=self.play_pause_image, compound="left", anchor="w")
        self.play_pause_image_label.bind("<Button-1>", callback)
        self.play_pause_image_label.grid(row=0, column=0, padx=5, pady=5, sticky="ew")

        self.stop_image = ctk.CTkImage(Image.open(os.path.join(image_path, "stop.png")), size=(40, 40))
        self.stop_image_label = ctk.CTkLabel(self.frame_buttons, text="", image=self.stop_image, compound="left", anchor="w")
        self.stop_image_label.grid(row=0, column=1, padx=5, pady=5, sticky="ew")

        self.entry_interval = ctk.CTkLabel(self.frame_buttons, text="Intervalo:")
        self.entry_interval.grid(row=0, column=2, padx=5, pady=5, sticky="ew")

        self.entry_interval = ctk.CTkEntry(self.frame_buttons, placeholder_text="15")
        self.entry_interval.grid(row=0, column=3, padx=5, pady=5, sticky="ew")

        self.combobox_unit = ctk.CTkComboBox(self.frame_buttons, state="readonly", values=["min", "seg"])
        self.combobox_unit.grid(row=0, column=4, padx=5, pady=5, sticky="ew")

        self.main_button_interval = ctk.CTkButton(master=self.frame_buttons, text="Enviar", fg_color="transparent", border_width=2, text_color=("gray10", "#DCE4EE"))
        self.main_button_interval.grid(row=0, column=5, padx=5, pady=5, sticky="ew")

class LogLine(ctk.CTkFrame):

    def __init__(self, master):
        super().__init__(master) 


   
class LogFrame(ctk.CTkScrollableFrame):

    def __init__(self, master):
        super().__init__(master) 

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=1)

        self.label_control = ctk.CTkLabel(self, text="Registro de datos", font=ctk.CTkFont(size=20, weight="bold"))
        self.label_control.grid(row=0, column=0, padx=(20, 10), pady=(10, 0), sticky="w")

        datalog = [
            ["Hora ", "  Fecha   ", "OD [%]", " pH  ", "Luz [%]", "Temperatura [°C]", " Ciclo  "],
            ["12:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["12:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["13:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["13:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["14:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["14:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["15:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["12:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["12:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["13:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["13:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["14:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["14:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["15:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["12:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["12:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["13:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["13:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["14:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["14:30", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"],
            ["15:00", "21/09/2024", " 350  ", "6.132", "  45   ", "      24.5      ", "Ciclo N1"]
        ]

        self.frame_lines = ctk.CTkFrame(self)
        self.frame_lines.grid(row=1, column=0, padx=(20, 10), pady=(10, 10), sticky="ew")
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
            self.frame_line.grid_columnconfigure(4, weight=1)
            self.frame_line.grid_columnconfigure(5, weight=1)
            self.frame_line.grid_columnconfigure(6, weight=1)

            self.label_time = ctk.CTkLabel(self.frame_line, text=datalog[i][0], corner_radius=0)
            self.label_time.grid(row=0, column=0, padx=5, pady=0, sticky="ew")

            self.label_date = ctk.CTkLabel(self.frame_line, text=datalog[i][1], corner_radius=0)
            self.label_date.grid(row=0, column=1, padx=5, pady=0, sticky="ew")

            self.label_od = ctk.CTkLabel(self.frame_line, text=datalog[i][2], corner_radius=0)
            self.label_od.grid(row=0, column=2, padx=5, pady=0, sticky="ew")

            self.label_ph = ctk.CTkLabel(self.frame_line, text=datalog[i][3], corner_radius=0)
            self.label_ph.grid(row=0, column=3, padx=5, pady=0, sticky="ew")

            self.label_light = ctk.CTkLabel(self.frame_line, text=datalog[i][4], corner_radius=0)
            self.label_light.grid(row=0, column=4, padx=5, pady=0, sticky="ew")

            self.label_temp = ctk.CTkLabel(self.frame_line, text=datalog[i][5], corner_radius=0)
            self.label_temp.grid(row=0, column=5, padx=5, pady=0, sticky="ew")

            self.label_cycle = ctk.CTkLabel(self.frame_line, text=datalog[i][6], corner_radius=0)
            self.label_cycle.grid(row=0, column=6, padx=5, pady=0, sticky="ew")

            if i==0:
                self.label_time.configure(font=ctk.CTkFont(size=15, weight="bold"))
                self.label_date.configure(font=ctk.CTkFont(size=15, weight="bold"))
                self.label_od.configure(font=ctk.CTkFont(size=15, weight="bold"))
                self.label_ph.configure(font=ctk.CTkFont(size=15, weight="bold"))
                self.label_light.configure(font=ctk.CTkFont(size=15, weight="bold"))
                self.label_temp.configure(font=ctk.CTkFont(size=15, weight="bold"))
                self.label_cycle.configure(font=ctk.CTkFont(size=15, weight="bold"))


class CycleFrame(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master, corner_radius=0, fg_color="transparent")

        self.grid_columnconfigure((0, 1), weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=5)
        
        self.instant_values_frame = ActualCycleFrame(self)
        self.instant_values_frame.grid(row=0, column=0, padx=10, pady=0, sticky="nsew")

        self.actual_cycle_frame = ControlCycleFrame(self)
        self.actual_cycle_frame.grid(row=0, column=1, padx=10, pady=(10, 0), sticky="nsew")

        self.log_frame = LogFrame(self)
        self.log_frame.grid(row=1, column=0, padx=10, pady=(10, 0), sticky="nsew", columnspan=2)

