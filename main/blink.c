/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "time.h"
#include <sys/time.h>

#define Sife_of_queue 8

void vOtherFunction( void );
void vTimer( void * pvParameters );
void vLogger( void * pvParameters );
int8_t Queue_SetItems (uint64_t data);
uint64_t Queue_GetItem();
int64_t Queue_GetSize();
void Logs(int64_t data);


struct tm tv;
time_t NOW;
struct timeval tv_now;

uint64_t time_pass;
uint64_t time_now;
uint64_t time_past;
uint64_t counter;

struct sec {
	__suseconds_t _sec_new;
	__suseconds_t _sec_old;
	__useconds_t _usec;
} timer;

struct Queue {
	int64_t data_1[Sife_of_queue];
	int64_t point;
} Queue_new;

/* Can use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO

void app_main(void)
{
    gpio_reset_pin(2);

    vOtherFunction();
    gpio_set_direction(2, GPIO_MODE_OUTPUT);

    Queue_new.point = 0;

    while(1) {
        gpio_set_level(2, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(2, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    }
}

void vTimer( void * pvParameters )
{
  for( ;; )
  {
	  time(&NOW);
	  localtime_r(&NOW, &tv);
	  time_now = tv.tm_sec;
	  Queue_SetItems(counter);
	  if ( time_now - time_past >= 5 ) {
		  time_past = time_now;
		  counter++;
	  }
      vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void vLogger( void * pvParameters )
{
  void (*logs_func_p)(int64_t data);
  logs_func_p = (void(*)(int64_t)) pvParameters;

  for( ;; )
  {
	  //int64_t data = Queue_GetItem();
		  logs_func_p(counter);
      vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void Logs(int64_t data) {
	  gettimeofday(&tv_now, NULL);
	  timer._sec_new = tv_now.tv_sec;
	  int32_t time_passed = timer._sec_new - timer._sec_old;
	  printf("Data = %lli\n", data);
	  printf("Time = %lli\n", (int64_t)time_passed * 1000000L + (int64_t)timer._usec);
	  printf("Time = %li\n", timer._usec);
	  timer._sec_old = timer._sec_new;
}
void vOtherFunction( void )
{
	xTaskCreatePinnedToCore(vLogger, "logger", 2048, (void*)Logs, 3, NULL, tskNO_AFFINITY);
	xTaskCreatePinnedToCore(vTimer, "timer", 2048, NULL, 2, NULL, tskNO_AFFINITY);
}

int8_t Queue_SetItems (uint64_t data1) {
	if (Queue_new.point < Sife_of_queue) {
		Queue_new.data_1[Queue_new.point] = data1;
		Queue_new.point++;
		return 0;
	} else  {
		return 1;
	}
}

uint64_t Queue_GetItem() {
	uint64_t return_data = 0;
	if (Queue_new.point >= 0) {
		return_data = Queue_new.data_1[0];
		for ( int64_t i = 1; i < Sife_of_queue; i++) {
			Queue_new.data_1[i - 1] = Queue_new.data_1[i];
		}
		if (Queue_new.point != 0) {
			Queue_new.point--;
		}
	} else {

	}
	return return_data;
}

int64_t Queue_GetSize() {
	int64_t return_data = Queue_new.point;
	return return_data;
}
