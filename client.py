# -*- coding: utf-8 -*-
import paho.mqtt.client as mqtt #Importa o módulo client do paho como mqtt (protocolo da camadade aplicação)
#Função de callback para conexão 
def on_connect(client, userdata, flags, rc):
	if rc ==0: #Se o código de retorno (rc) for 0, houve conexão
		print("Conectado") 
	else: 
		print(f"Code: {rc}") #Senão, exibe o código de retorno a fim de verificar o erro 
	t = "MQTTReceiveDistance"
	while True: 
		q = 0
		if 0<=q<=2: #O qual deve estrar entre 0 e 2
			break #Dessa forma, quebre o laço, senão continue até o parâmetro estar correto 
	client.subscribe(topic = t, qos=q) #inscreve o cliente no topico t com qos q 
#Função de callback referente ao recebimento de mensagens	
def on_message(client, userdata, message):
	print('______________________')
	print(f'topic: {message.topic}') #exibe o nome do tópico
	print(f'distance: {message.payload.decode()}') #exibe a mensagem recebida em unicode
	print(f'qos: {message.qos}') #exibe o nível de qos da mensagem
	print('-----------------------')
#broker = "127.0.0.1"
broker = "mqtt.eclipse.org"
port = 1883 
client = mqtt.Client(client_id="default", clean_session=False)#Gera uma instância do cliente representado pelo id "default"
client.on_connect = on_connect#Realiza o bind da função de callback on_connect
client.on_message = on_message#Realia o bind da função de callback on_message
client.connect(host=broker, port=port)#Conecta-se ao broker
try:
	client.loop_forever()#Necessário para startar as função de callback
except:
	print('Erro inesperado ... Abortando processo')#Trativa de erro  



