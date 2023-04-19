#include <stdio.h>
#include <inttypes.h>
#include "lcdv2r6.h"
#include "gpiov2r3.h"
#include "keyv1r2.h"
#include "wifiv1r1.h"
#include "geralv2r1.h"
#define BROKER_URI "mqtt://test.mosquitto.org"
#define MQTT_PORT 1883
#define MQTT_TOPIC "test/topic"
#define MQTT_CLIENT_ID "esp32"



uint32_t MQTT_CONNEECTED = 0;


static void mqtt_app_start(void);

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT CONECTADO COM SUCESSO");
        MQTT_CONNEECTED=1;
        msg_id = esp_mqtt_client_subscribe(client, MQTT_TOPIC, 0); 
        ESP_LOGI(TAG, "Mensagem enviada com sucesso, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT DESCONECTADO");
        MQTT_CONNEECTED=0;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MENSAGEM ENVIADA, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "Evento cancelado, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "Mensagem publicada, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MENSAGEM RECEBIDA");
        ESP_LOGI(TAG, "MENSAGEM ENVIADA, DATA=%.*s\r\n", event->data_len, event->data);
         if (strncmp(event->data, "on", event->data_len) == 0) 
    {
        vTaskDelay(1000);
        gpoDado(0b11111111);
        lcdTexto("GPO = 0XFF",1,1);
    }

         if (strncmp(event->data, "off", event->data_len) == 0) 
    {
        gpoDado(0b0000000);
        lcdTexto("GPO = 0X00",1,1);
    }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT ERRO!");
        break;
    default:
        ESP_LOGI(TAG, "Outro evento:%d", event->event_id);
        break;
    }
}

esp_mqtt_client_handle_t client = NULL;
static void mqtt_app_start(void)
{
    ESP_LOGI(TAG, "Iniciando MQTT");
    esp_mqtt_client_config_t mqtt_Config = {
        .broker.address.uri = BROKER_URI,
        .broker.address.port = MQTT_PORT,
        
    };
    
    client = esp_mqtt_client_init(&mqtt_Config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

void Publisher_Task(void *params)
{
  while (true)
  {
    if(MQTT_CONNEECTED)
    {
        esp_mqtt_client_publish(client, MQTT_TOPIC, "on", 0, 0, 0); 
        vTaskDelay(3000);
        esp_mqtt_client_publish(client, MQTT_TOPIC, "off", 0, 0, 0);
        vTaskDelay(1000);
    }
  }
}

void app_main(void)
{
    gpoIniciar();
    lcdIniciar();
    wifi_init();
    mqtt_app_start();
    xTaskCreate(Publisher_Task, "Publisher_Task", 1024 * 5, NULL, 5, NULL);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);   


// Aguardar eventos
while(1) 
{
    
}
    

}
