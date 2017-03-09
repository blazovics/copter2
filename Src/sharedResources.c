#include "sharedResources.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static SemaphoreHandle_t dummyHandler;
static int dummyData = 0;

void initializeResources() {
	dummyHandler = xSemaphoreCreateBinary();
}

int getDummyData() {
	xSemaphoreTake(dummyHandler, portMAX_DELAY);
	int copy = dummyData;
	xSemaphoreGive(dummyHandler);
	return copy;
}

void setDummyData(int data) {
	xSemaphoreTake(dummyHandler, portMAX_DELAY);
	dummyData = data;
	xSemaphoreGive(dummyHandler);
}
