
#include <stdio.h>
#include <stdlib.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
//#include "esp_camera.h"
#include "driver/i2c_master.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include <esp_https_server.h>
#include "esp_tls.h"
#include "protocol_examples_common.h"
#include "sdkconfig.h"

#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_SCL_IO           27                // replace with appropriate GPIO pin
#define I2C_MASTER_SDA_IO           26                // replace with appropriate GPIO pin
#define I2C_MASTER_FREQ_HZ          400000            // I2C clock frequency

static const char *TAG = "Camera";
#define MOUNT_POINT "/sdcard"
#define BOARD_ESP32CAM_AITHINKER 1

// ESP32Cam (AiThinker) PIN Map
#ifdef BOARD_ESP32CAM_AITHINKER

#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1 //software reset will be performed
//#define CAM_PIN_XCLK 0
//#define CAM_PIN_SIOD 26
//#define CAM_PIN_SIOC 27
#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#endif


#define AP_SSID "ESPTEST"
#define AP_PASSWORD "12345678"
#define AP_MAX_CONN 2
#define AP_CHANNEL 0
#define STA_SSID "ShaheenTel-B31 F1"
#define STA_PASSWORD "5DF5AD90"
#define EXAMPLE_ESP_MAXIMUM_RETRY 100
static const char *TAG1 = "httpd";







/*static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    //.pin_sscb_sda = CAM_PIN_SIOD,
    //.pin_sscb_scl = CAM_PIN_SIOC,
    .pin_sscb_sda = I2C_MASTER_SDA_IO,
    .pin_sscb_scl = I2C_MASTER_SCL_IO,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 10000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG, //YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_UXGA,    //QQVGA-UXGA Do not use sizes above QVGA when not JPEG

    .jpeg_quality = 19, //0-63 lower number means higher quality
    .fb_count = 1       //if more than one, i2s runs in continuous mode. Use only with JPEG
};*/


/*static void i2c_master_init() {
    i2c_master_bus_config_t conf ={
        //conf.i2c_mode_t = I2C_MODE_MASTER;
         conf.sda_io_num = I2C_MASTER_SDA_IO,
        //conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
         conf.flags.enable_internal_pullup = 1,
         conf.scl_io_num = I2C_MASTER_SCL_IO,
         // conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
         //conf.clk_source = I2C_MASTER_FREQ_HZ};

    //i2c_param_config(I2C_MASTER_NUM, &conf);
   // i2c_driver_install(I2C_MASTER_NUM, conf.i2c_mode_t, 0, 0, 0);
}
static esp_err_t init_camera()
{
    //initialize the camera
    i2c_master_init();
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }

    return ESP_OK;
}*/


/*static void init_sdcard()
{
  esp_err_t ret = ESP_FAIL;

  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024
  };
  sdmmc_card_t *card;

  const char mount_point[] = MOUNT_POINT;
  ESP_LOGI(TAG, "Initializing SD card");

  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

  ESP_LOGI(TAG, "Mounting SD card...");
  gpio_set_pull_mode(15, GPIO_PULLUP_ONLY);   // CMD, needed in 4- and 1- line modes
  gpio_set_pull_mode(2, GPIO_PULLUP_ONLY);    // D0, needed in 4- and 1-line modes
  gpio_set_pull_mode(4, GPIO_PULLUP_ONLY);    // D1, needed in 4-line mode only
  gpio_set_pull_mode(12, GPIO_PULLUP_ONLY);   // D2, needed in 4-line mode only
  gpio_set_pull_mode(13, GPIO_PULLUP_ONLY);   // D3, needed in 4- and 1-line modes

  ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

  if (ret == ESP_OK)
  {
    ESP_LOGI(TAG, "SD card mount successfully!");
  }
  else
  {
    ESP_LOGE(TAG, "Failed to mount SD card VFAT filesystem. Error: %s", esp_err_to_name(ret));
  }

  // Card has been initialized, print its properties
  // sdmmc_card_print_info(stdout, card);

}*/

// Server
static esp_err_t server_get_handler(httpd_req_t *req)
{
    const char resp[] = "Server GET Response .................";
    httpd_resp_set_type(req, "text/html");
   // httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_send(req, "Hello SHharukh", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t server_post_handler(httpd_req_t *req)
{
    char content[100];
    size_t recv_size = MIN(req->content_len, sizeof(content));
    int ret = httpd_req_recv(req, content, recv_size);

    // If no data is send the error will be:
    // W (88470) httpd_uri: httpd_uri: uri handler execution failed
    printf("\nServer POST content: %s\n", content);

    if (ret <= 0)
    { //0 return value indicates connection closed 
        // Check if timeout occurred 
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
     //Send a simple response 
    const char resp[] = "Server POST Response .................";
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

static const httpd_uri_t server_uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = server_get_handler
};

static const httpd_uri_t server_uri_post = {
    .uri = "/",
    .method = HTTP_POST,
    .handler = server_post_handler
};


static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server");

    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();

    extern const unsigned char servercert_start[] asm("_binary_servercert_pem_start");
    extern const unsigned char servercert_end[]   asm("_binary_servercert_pem_end");
    conf.servercert = servercert_start;
    conf.servercert_len = servercert_end - servercert_start;

    extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
    extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");
    conf.prvtkey_pem = prvtkey_pem_start;
    conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;

#if CONFIG_EXAMPLE_ENABLE_HTTPS_USER_CALLBACK
    conf.user_cb = https_server_user_callback;
#endif
    esp_err_t ret = httpd_ssl_start(&server, &conf);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG1, "Error starting server!");
        return NULL;
    }

    // Set URI handlers
    ESP_LOGI(TAG1, "Registering URI handlers");
    httpd_register_uri_handler(server, &server_uri_get);
    httpd_register_uri_handler(server, &server_uri_post);
    return server;
}
    static esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
     return httpd_ssl_stop(server);
}

// Client
/*esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("Client HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}*/

/*static void client_post_rest_function()
{
    esp_http_client_config_t config_post = {
        .url = "https://console.firebase.google.com/u/0/project/espcam-dc23e/firestore/databases/-default-/data/~2Fdocuments~2Fmycollection?hl=fr&view=panel-view&scopeType=collection&scopeName=%2Fdocuments&query=",
        .method = HTTP_METHOD_POST,
        .cert_pem = (const char *)ClientCert_pem_start,
        .event_handler = client_event_get_handler};
        
    esp_http_client_handle_t client = esp_http_client_init(&config_post);

    char  *post_data = "{\"fields\":{\"Memory\":{\"doubleValue\":\"2\"},\"Name\":{\"stringValue\":\"Data from the client\"}}}";
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_http_client_set_header(client, "Content-Type", "application/json");

    esp_http_client_perform(client);
    esp_http_client_cleanup(client);





}*/

/*wifi_config_t sta_config = {
.sta = {
.ssid= STA_SSID,
.password = STA_PASSWORD,
.failure_retry_cnt= EXAMPLE_ESP_MAXIMUM_RETRY
},
};


    wifi_config_t ap_config = {
    .ap = {
    .ssid = AP_SSID,
    .password = AP_PASSWORD,
    .max_connection= AP_MAX_CONN,
    .channel = AP_CHANNEL,
    .ssid_hidden = 0,
    },
    };
*/

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        *server = start_webserver();
    }
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop https server");
        }
    }
}


void app_main(void)
{
    /*init_camera();
    init_sdcard();

    while(1)
    {

        ESP_LOGI(TAG, "Taking picture...");
        camera_fb_t *pic = esp_camera_fb_get();
        static uint64_t counter = 0;
        counter++;
        printf(" the counter is increasing to %lli \n ",counter);

        char *pic_name = malloc(30 + sizeof(int64_t));
        sprintf(pic_name, MOUNT_POINT"/pic_%lli.jpg ", counter);

        FILE *file = fopen("/sdcard/pic_1.jpg", "rb");

        if (file == NULL) 
        {
            ESP_LOGE(TAG, "Failed to open image file");
        }
        else 
        {
            printf("image has been red succesfully =D \n");

        }

        fclose(file);
        free(pic_name);
        printf("ok1\n");

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("ok2\n");*/

        /*FILE *file = fopen(pic_name, "w");
       

        if (file != NULL)
        {
            fwrite(pic->buf, 1, pic->len, file);
            ESP_LOGI(TAG, "File saved: %s", pic_name);
        }
        else
        {
            ESP_LOGE(TAG, "Could not open file =(");
        }
        fclose(file);
        esp_camera_fb_return(pic);
        free(pic_name);
        printf("ok1\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("ok2\n");

    }*/  

    httpd_handle_t server = NULL;

    // initialize NVS
    ESP_ERROR_CHECK(nvs_flash_init());
    esp_netif_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
  
  

    //wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    //ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));
    //ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    //ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA,&sta_config));

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

    //ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP,&ap_config));
    //ESP_ERROR_CHECK(esp_wifi_start());// starts wifi usage
    //ESP_ERROR_CHECK(esp_wifi_connect());
    ESP_ERROR_CHECK(example_connect());
    
    

    
   // start_webserver();

   
    //vTaskDelay(2000 / portTICK_PERIOD_MS);
    
  

}