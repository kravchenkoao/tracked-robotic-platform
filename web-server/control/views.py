from django.shortcuts import render
from django.views.decorators.csrf import ensure_csrf_cookie
from django.http import HttpResponse
from serial import Serial
import json

s = Serial('COM6', baudrate=9600) # connection to external dev board with transmitter

# Create your views here.
@ensure_csrf_cookie #secure connection
def index(request):
	if request.method == 'POST':
		jsonData = json.loads(request.body)
		joy1PosX = jsonData.get('joy1PosX')
		joy1PosY = jsonData.get('joy1PosY')
		joy2PosX = jsonData.get('joy2PosX')
		joy2PosY = jsonData.get('joy2PosY')
		print(joy1PosX, joy1PosY, joy2PosX, joy2PosY)
		s.write(f'{joy1PosX} {joy1PosY} {joy2PosX} {joy2PosY}\n'.encode('utf-8'))
		print(f'{joy1PosX} {joy1PosY} {joy2PosX} {joy2PosY}')
	return render(request, 'control/index.html')

def silent(request): #sending empty message
	pass
	# print(f'silent')
	# s.write('0/n'.encode('utf-8'))
	# return HttpResponse(200)