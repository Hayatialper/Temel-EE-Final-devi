import serial
import speech_recognition as sr

# Seri bağlantıyı yapılandırın
ser = serial.Serial('COMX', 115200)
""" COMX, ESP32'nin bağlı olduğu seri portu belirtmelisiniz. 115200 Baud değeri
esp32 için geçerli eğer bu kodu arduino için kullanıyorsanız 9600 Baud yeterli."""

# Tanımlanan seri bağlantı üzerinden veri gönderme fonksiyonu
def send_data(data):
    ser.write(data.encode('utf-8'))

# Mikrofonu dinleme fonksiyonu
def listen_microphone():
    r = sr.Recognizer()
    with sr.Microphone() as source:
        print("Mikrofon dinleniyor...")
        audio = r.listen(source)

    try:
        print("Ses tanınıyor...")
        text = r.recognize_google(audio, language='tr-TR')  # Türkçe dilinde sesi tanıma
        print("Tanınan metin:", text)
        send_data(text)  # Tanınan metni ESP32'ye iletiyoruz
    except sr.UnknownValueError:
        print("Ses anlaşılamadı")
    except sr.RequestError as e:
        print("Ses tanıma servisi çalışmıyor; {0}".format(e))

# Ana döngü
while True:
    listen_microphone()
