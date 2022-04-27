import smtplib
from email.mime.text import MIMEText

# sender's mail
sender = "sherlockholmes@gmail.com"
# receiver's mail
receivers = ["johnwatson@gmil.com"]

port = 5000

# message to be sent
message = MIMEText("Hurry up, John!")

message["subject"] = "SOS"
message["from"] = "sherlockholmes@gmail.com"
message["to"] = "johnwatson@gmil.com"

with smtplib.SMTP("localhost", port) as server:
    server.sendmail(sender, receivers, message.as_string())
    print("Message sent!")
