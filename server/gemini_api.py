from datetime import datetime, timezone, timedelta

from google import genai
from google.genai import types

from config import Gemini_key

client = genai.Client(api_key=Gemini_key)

def ask(text,sensor):
    WIB=timezone(timedelta(hours=7))
    now = datetime.now(WIB)

    tanggal = now.strftime("%d-%m-%Y")
    waktu = now.strftime("%H:%M:%S")

    system_instruction =f""" 
        kamu adalah sebuah smart assistant milik pribadi

        aturan jawaban:
        - Jawab dalam bahasa Indonesia.
        - Jawab secara singkat dan langsung.
        - Maksimal 2 kalimat.
        - Gunakan maksimal 20 kata.
        - Jangan memberikan penjelasan tambahan jika tidak diminta.
        - Jika menggunakan Google Search, ambil informasi yang paling relevan saja.
        - Jangan menampilkan daftar sumber atau hasil pencarian kecuali pengguna memintanya.
        - Jangan mengulangi pertanyaan pengguna.   

        Tanggal saat ini: {tanggal}
        Waktu saat ini: {waktu}
        Zona waktu: WIB (UTC+7).

        Gunakan tanggal dan waktu tersebut jika pengguna menanyakan tanggal atau waktu.
        
        jika tidak ditanya abaikan Data sensor:{sensor}
        if pertanyaan membutuhkan informasi terkini seperti harga, berita, cuaca,
        nilai tukar, jadwal, atau informasi yang dapat berubah dari waktu ke waktu,
        gunakan Google Search.
        """
    response = client.models.generate_content(
        model="gemini-2.5-flash-lite",
        contents=text,
        config=types.GenerateContentConfig(system_instruction=system_instruction,max_output_tokens=35,tools=[types.Tool(google_search=types.GoogleSearch)]),
    )
    return(response.text)

