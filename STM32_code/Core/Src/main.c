/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stm32f1_rc522.h"
#include "string.h"
#include <stdint.h>	//for addName()
#include <stdbool.h>
#include <stdlib.h>	//for free() Freeing the dynamically allocated memory
#include "cmox_crypto.h"	//from st's cryptolib, allows for aes encryption and decryption
#include <math.h> //for use in calculating rsa
#include "rsa.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFFER_LEN 500	//BLUETUT
#define MAX_NAMES 500
#define MAX_NAME_LENGTH  50
#define subArraySize 15
#define TIMESTAMP_LENGTH  20
#define CHUNK_SIZE  48u   /* Chunk size (in bytes) when data to encrypt or decrypt are processed by chunk */
#define CBC 1	//Enable CBC mode for aes
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

//============================================ RFID-rc522 ============================================
//uint8_t i;
uint8_t status;
uint8_t str[MAX_LEN]; // Max_LEN = 16
uint8_t serNum[5];

uint8_t  rfidKey[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uint8_t test;
uint8_t W[]="zagros-elec";
uint8_t R1[15];
uint8_t R2[15];
uint8_t R3[15];
uint8_t combinedR[45];


uint8_t writeArrayDonor[MAX_NAME_LENGTH];
uint8_t writeArray1[subArraySize];
uint8_t writeArray2[subArraySize];
uint8_t writeArray3[subArraySize];

//============================================ Bluetooth HC05 ============================================
uint8_t TX_BUFFER[BUFFER_LEN] = {0};
uint8_t RX_BUFFER[BUFFER_LEN] = {0};
int newMessageFlag = 0;	//ensures that the code executed by bluetooth message is executed only once

//============================================ Counters, Flags and structures ============================================
int i = 0;
int nameSpaceCounter = 0;	//counts how many chars of package are actually a name
struct NamesArray {
    uint8_t names[MAX_NAMES][MAX_NAME_LENGTH];
    size_t count;
};
struct HistoryArray {
    uint8_t names[MAX_NAME_LENGTH];
    uint8_t timeStamps[TIMESTAMP_LENGTH];
};
int historyEntranceCounter = 0;	//used to add each entrance to the next array member and not overwrite the last
int readFlag = 1;	//indicated if the reading the rfid card is currently turned on
int writeFlag = 0;	//indicated if the writing to the rfid card is currently turned on
uint8_t nameToWrite[MAX_NAME_LENGTH];	//the name passed with "writeOn" command to be written on the rfid card
char bufferForOutput[400];

//============================================ RTC ============================================
RTC_TimeTypeDef time;
RTC_DateTypeDef date;

//============================================ test ============================================
int testlen;
long long testarray[64]={0};
long long *encryptedtest;
char *decryptedtest;
uint8_t testBigBuffer[400];
char *decryptedMessage;

//============================================ CRYPTOLIB - AES ============================================
cmox_cbc_handle_t Cbc_Ctx;	//CBC context handle
volatile TestStatus glob_status = FAILED;

//============================================ RSA ============================================
struct public_key_class pubSTM[1];
struct private_key_class privSTM[1];
struct public_key_class pubAPP[1];
long long numberpe, numberpm;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == huart1.Instance)
	{
		HAL_UART_Receive_IT(&huart1, RX_BUFFER, BUFFER_LEN);
		newMessageFlag = 1;		//ensures that the code executed by bluetooth message is executed only once
	}
}

void addName(struct NamesArray *array, const uint8_t *buffer, size_t start, size_t length) {
    if (array->count < MAX_NAMES && length < MAX_NAME_LENGTH) {
        strncpy(array->names[array->count], buffer + start, length);
        array->names[array->count][length] = '\0';  // Ensure null-terminated string
        array->count++;
    }
}

void deleteName(struct NamesArray *array, const uint8_t *nameToDelete) {
    for (size_t i = 0; i < array->count; ++i) {
        if (strcmp(array->names[i], nameToDelete) == 0) {
            // Move names after the deleted name to fill the gap
            for (size_t j = i; j < array->count - 1; ++j) {
                strcpy(array->names[j], array->names[j + 1]);
            }
            array->count--;
            break;  // Exit the loop after deleting the name
        }
    }
}

void SaveForLaterSomeEncryptingDecrypting(char *stingToSend){
	/*//printf("Private Key:\n Modulus: %lld\n Exponent: %lld\n", (long long)priv->modulus, (long long) priv->exponent);
										strcpy(bufferForOutput, "Private Key Exponent: ");
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
										sprintf(bufferForOutput, "%d\n", priv->exponent);
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
										strcpy(bufferForOutput, "Public Key Exponent: ");
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
										sprintf(bufferForOutput, "%d\n", pub->exponent);
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);

										strcpy(bufferForOutput, "pe:");
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
										sprintf(bufferForOutput, "%lu\n", pubAPP->exponent);
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
										HAL_Delay(100);
										strcpy(bufferForOutput, "pm:");
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
										sprintf(bufferForOutput, "%lu\n", pubAPP->modulus);
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
										HAL_Delay(100);
		char message[] = "hello, Naomi!";
		int i;

		printf("Original:\n");
		for(i=0; i < strlen(message); i++){
		printf("%lld\n", (long long)message[i]);
		}
										strcpy(bufferForOutput, "Original: ");
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
										for(i=0; i < strlen(message); i++){
										sprintf(bufferForOutput, "%d ", (long long)message[i]);
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
										}
										strcpy(bufferForOutput, "\n");
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);

		long long *encrypted = rsa_encrypt(message, sizeof(message), pubAPP);
		if (!encrypted){fprintf(stderr, "Error in encryption!\n");}
										strcpy(bufferForOutput, "e:");
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
										for(i=0; i < strlen(message); i++){
										sprintf(bufferForOutput, "%lu ", (long long)encrypted[i]);
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
										}
										strcpy(bufferForOutput, "\n");
										HAL_Delay(100);
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
		char *decrypted = rsa_decrypt(encrypted, 8*sizeof(message), privSTM);
		if (!decrypted){fprintf(stderr, "Error in decryption!\n");}
										strcpy(bufferForOutput, "Decrypted: ");
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
										for(i=0; i < strlen(message); i++){
										sprintf(bufferForOutput, "%c ", (long long)decrypted[i]);
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
										}
										strcpy(bufferForOutput, "\n");
										HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);

		free(encrypted);
		free(decrypted);

*/
}


void sendEncryptedData(char *messageToSend){

    char message[200]; // Assuming a maximum length for the message

    // Copy the content of messageToSend into message
    strcpy(message, messageToSend);
	int i;

	printf("Original:\n");
	for(i=0; i < strlen(message); i++){
	printf("%lld\n", (long long)message[i]);
	}


	long long *encrypted = rsa_encrypt(message, sizeof(message), pubAPP);
	if (!encrypted){fprintf(stderr, "Error in encryption!\n");}
									strcpy(bufferForOutput, "e:");
									HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
									for(i=0; i < strlen(message); i++){
									sprintf(bufferForOutput, "%lu ", (long long)encrypted[i]);
									HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
									}
									strcpy(bufferForOutput, "\n");
									HAL_Delay(150);
									HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);


	free(encrypted);


}


long long *extractNumbers(const uint8_t *buffer, size_t *numElements) {
    const char *startMarker = "m:";
    const char *endMarker = "}";
    size_t startMarkerLength = strlen(startMarker);
    size_t endMarkerLength = strlen(endMarker);

    // Find the position of "m:" in the buffer
    const char *startPos = strstr((const char *)buffer, startMarker);
    if (startPos == NULL) {
        fprintf(stderr, "Error: Start marker not found.\n");
        return NULL;
    }

    // Move the position after "m:"
    startPos += startMarkerLength;

    // Find the position of "}" in the buffer
    const char *endPos = strstr(startPos, endMarker);
    if (endPos == NULL) {
        fprintf(stderr, "Error: End marker not found.\n");
        return NULL;
    }

    // Calculate the length between "m:" and "}"
    size_t length = endPos - startPos;

    // Calculate the number of elements
    *numElements = length / 10;

    // Allocate memory for the array
    long long *message = malloc(sizeof(long long) * (*numElements));
    if (message == NULL) {
        fprintf(stderr, "Error: Heap allocation failed.\n");
        return NULL;
    }

    // Extract numbers and convert to long long
    for (size_t i = 0; i < *numElements; i++) {
        char numberString[11];
        strncpy(numberString, startPos + i * 10, 10);
        numberString[10] = '\0';

        message[i] = strtoll(numberString, NULL, 10);
    }

    return message;
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  MFRC522_Init();

  HAL_UART_Receive_IT(&huart1, RX_BUFFER, BUFFER_LEN);	//BLUETUT

  //HAL_Delay(1000);
  //HAL_TIM_Base_Start_IT(&htim2);
  TX_BUFFER[0]=4;

  struct HistoryArray myHistory[MAX_NAMES]; // this structures stores the history later passed to the app
  struct NamesArray myNames = {{0}, 0}; // this structures stores the names which are allowed to enter
  nameSpaceCounter = i - 5;
  char masterBuffer[] = "master";
  addName(&myNames, masterBuffer, 0, 6);

  RTC_TimeTypeDef new_time = {0};
  new_time.Hours = 7;
  new_time.Minutes = 45;
  new_time.Seconds = 0;
  HAL_RTC_SetTime(&hrtc, &new_time, RTC_FORMAT_BIN);
  newMessageFlag = 0;		//ensures that the code executed by bluetooth message is executed only once
  RTC_DateTypeDef today;
  today.Year = 21;
  today.Month = 06;
  today.Date = 1;
  today.WeekDay = RTC_WEEKDAY_TUESDAY;
  HAL_RTC_SetDate(&hrtc, &today, RTC_FORMAT_BIN);


  rsa_gen_keys(pubSTM, privSTM, PRIME_SOURCE_FILE);	//generating both public and private rsa keys
  char messagetest[] = "TestMessageUwU";
  encryptedtest = rsa_encrypt(messagetest, sizeof(messagetest), pubSTM);
  decryptedtest = rsa_decrypt(encryptedtest, 8*sizeof(messagetest), privSTM);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);


	  	  	//HAL_UART_Receive_IT(&huart1, RX_BUFFER, BUFFER_LEN);	//BLUETUT
	  	  	MFRC522_Init();		//reset is needed for rfid module to work, also needs to be not in rfid loop as not to cause rst pin problem
	  	  	HAL_GPIO_WritePin(GPIOB, Bluetooth_LED_Pin, GPIO_PIN_RESET);
	  	  	status = MFRC522_Request(PICC_REQIDL, str);	//MFRC522_Request(0x26, str)
	  		status = MFRC522_Anticoll(str);//Take a collision, look up 5 bytes
	  		memcpy(serNum, str, 5);//function for c language:(para1:that place save data,para2:the the source of data,para3:size)
	  		if(readFlag == 1){
	  			HAL_GPIO_WritePin(GPIOB, RFID_LED_Pin, GPIO_PIN_SET);
	  		}else{
	  			HAL_GPIO_WritePin(GPIOB, RFID_LED_Pin, GPIO_PIN_RESET);
	  		}
	  		if(writeFlag == 1){
	  			HAL_GPIO_WritePin(GPIOB, RFID_WRITE_LED_Pin, GPIO_PIN_SET);
	  		}else{
	  			HAL_GPIO_WritePin(GPIOB, RFID_WRITE_LED_Pin, GPIO_PIN_RESET);
	  		}


	  		if (status == MI_OK && readFlag == 1)
	  		{
	  				//HAL_GPIO_WritePin(RFID_LED_GPIO_Port, RFID_LED_Pin, 1);
	  				//HAL_GPIO_WritePin(GPIOB, Bluetooth_LED_Pin, GPIO_PIN_SET);
					HAL_Delay(200);
					MFRC522_SelectTag(str);
					test =	MFRC522_Auth(PICC_AUTHENT1A,2,rfidKey,serNum);
					status = MFRC522_Read( 2, R1);
					test =	MFRC522_Auth(PICC_AUTHENT1A,4,rfidKey,serNum);
					status = MFRC522_Read( 4, R2);
					test =	MFRC522_Auth(PICC_AUTHENT1A,5,rfidKey,serNum);
					status = MFRC522_Read( 5, R3);

					memcpy(combinedR, R1, sizeof(R1));
					memcpy(combinedR + sizeof(R1), R2, sizeof(R2));
					memcpy(combinedR + 2 * sizeof(R2), R3, sizeof(R3));

					i = 0;
					while( combinedR[i]!=' '){	//finding how many chars does the name takes before a space-stopper
						i++;
					}
					nameSpaceCounter = i;

					for (size_t i = 0; i < myNames.count; ++i) {
						if (strncmp(myNames.names[i], combinedR, nameSpaceCounter) == 0) {
							HAL_GPIO_WritePin(GPIOB, Bluetooth_LED_Pin, GPIO_PIN_SET);
							HAL_Delay(2000);
							HAL_GPIO_WritePin(GPIOB, Bluetooth_LED_Pin, GPIO_PIN_RESET);
							// Open when there's a match

							snprintf((char*)myHistory[historyEntranceCounter].timeStamps, sizeof(myHistory[historyEntranceCounter].timeStamps), "%02d-%02d-%02d/%02d:%02d", date.Year, date.Month, date.Date, time.Hours, time.Minutes);
							snprintf((char*)myHistory[historyEntranceCounter].names, sizeof(myHistory[historyEntranceCounter].names), "%s:", myNames.names[i]);
							historyEntranceCounter++;


							break;  // If you want to stop after the first match
						}
					}
	  		}
	  		if (status == MI_OK && writeFlag == 1)
	  		{
	  			HAL_Delay(500);
	  			MFRC522_SelectTag(str);
	  			test =	MFRC522_Auth(PICC_AUTHENT1A,2,rfidKey,serNum);
	  			status = MFRC522_Write((uint8_t)2 , writeArray1);
	  			test =	MFRC522_Auth(PICC_AUTHENT1A,4,rfidKey,serNum);
	  			status = MFRC522_Write((uint8_t)4 , writeArray2);
	  			test =	MFRC522_Auth(PICC_AUTHENT1A,5,rfidKey,serNum);
	  			status = MFRC522_Write((uint8_t)5 , writeArray3);
	  		}


	  			//OPEN
	          if(newMessageFlag == 1 && RX_BUFFER[0] == '{' && RX_BUFFER[1] == 'o' && RX_BUFFER[2] == 'p' && RX_BUFFER[3] == 'e' && RX_BUFFER[4] == 'n')
	          {
	        	  sendEncryptedData("opening door");
	        	  HAL_GPIO_WritePin(GPIOB, Bluetooth_LED_Pin, GPIO_PIN_SET);
	        	  HAL_Delay(1000);
	        	  HAL_GPIO_WritePin(GPIOB, Bluetooth_LED_Pin, GPIO_PIN_RESET);
	        	  newMessageFlag = 0;		//ensures that the code executed by bluetooth message is executed only once
	          }
	          //READING START
	          if(newMessageFlag == 1 && RX_BUFFER[0] == '{' && RX_BUFFER[1] == 'r' && RX_BUFFER[2] == 'e' && RX_BUFFER[3] == 'a' && RX_BUFFER[4] == 'd' && RX_BUFFER[5] == 'O' && RX_BUFFER[6] == 'n')
	          {
	              sendEncryptedData("starting reading");
	              readFlag = 1;
	              writeFlag = 0;
	              newMessageFlag = 0;		//ensures that the code executed by bluetooth message is executed only once
	          }

	          //READING STOP
	          if(newMessageFlag == 1 && RX_BUFFER[0] == '{' && RX_BUFFER[1] == 'r' && RX_BUFFER[2] == 'e' && RX_BUFFER[3] == 'a' && RX_BUFFER[4] == 'd' && RX_BUFFER[5] == 'O' && RX_BUFFER[6] == 'f')
			  {
	              sendEncryptedData("stopping reading");
				  readFlag = 0;
				  newMessageFlag = 0;		//ensures that the code executed by bluetooth message is executed only once
			  }

	          //ADD USER
	          if(newMessageFlag == 1 && RX_BUFFER[0] == '{' && RX_BUFFER[1] == 'A' && RX_BUFFER[2] == 'd' && RX_BUFFER[3] == 'd' && RX_BUFFER[4] == ':')
	          {
	        	  i = 0;
	        	  while( RX_BUFFER[i]!='}'){
	        		  i++;
	        	  }
	        	  nameSpaceCounter = i - 5;
	        	  addName(&myNames, RX_BUFFER, 5, nameSpaceCounter);	//nameSpaceCounter is length of the name and 5 is index where the name starts in buffer
	        	  newMessageFlag = 0;		//ensures that the code executed by bluetooth message is executed only once
	       	  }

	          //DELETE USER
			  if(newMessageFlag == 1 && RX_BUFFER[0] == '{' && RX_BUFFER[1] == 'D' && RX_BUFFER[2] == 'l' && RX_BUFFER[3] == 't' && RX_BUFFER[4] == ':')
			  {
				  i = 0;
				  while( RX_BUFFER[i]!='}'){
					  i++;
				  }
				  nameSpaceCounter = i - 5;
				  uint8_t nameToDelete[nameSpaceCounter + 1];  // +1 for null-terminator
				  memcpy(nameToDelete, RX_BUFFER + 5, nameSpaceCounter);
				  nameToDelete[nameSpaceCounter] = '\0';  // Null-terminate the string
				  //const uint8_t *nameToDelete = "igor";
				  deleteName(&myNames, nameToDelete);
				  newMessageFlag = 0;		//ensures that the code executed by bluetooth message is executed only once
			  }

	          //SYNCHRONIZE USERS
	          if(newMessageFlag == 1 && RX_BUFFER[0] == '{' && RX_BUFFER[1] == 's' && RX_BUFFER[2] == 'y' && RX_BUFFER[3] == 'n' && RX_BUFFER[4] == 'c' && RX_BUFFER[5] == 'U' && RX_BUFFER[6] == 's' && RX_BUFFER[7] == 'e' && RX_BUFFER[8] == 'r' && RX_BUFFER[9] == '}')
	          {
	        	  char buffer1[] = "usr:";
	        	  HAL_UART_Transmit(&huart1, (void*)buffer1, strlen(buffer1), 1000);
	        	  for(i = 0; i < myNames.count; i++){
	        		  HAL_UART_Transmit(&huart1, (void*)myNames.names[i], strlen(myNames.names[i]), 1000);
	        		  char buffer[] = " ";
	        		  HAL_UART_Transmit(&huart1, (void*)buffer, strlen(buffer), 1000);
	        	  }
				  char buffer2[] = "\n";
				  HAL_UART_Transmit(&huart1, (void*)buffer2, strlen(buffer2), 1000);



				  sendEncryptedData("UsersStart");
				  HAL_Delay(50);
				  for(i = 0; i < myNames.count; i++){
					  char buffer[MAX_NAME_LENGTH + TIMESTAMP_LENGTH + 1]; // +1 for null terminator

					  // Concatenate names and timeStamps into the buffer
					  snprintf(buffer, sizeof(buffer), "u:%s ", myNames.names[i]);

					  // Transmit the concatenated string
					  sendEncryptedData(buffer);
				  }
				  sendEncryptedData("UsersEnd");




				  newMessageFlag = 0;		//ensures that the code executed by bluetooth message is executed only once

	          }
	          HAL_Delay(500);

	          //TIME SYNCHRONIZATION
	          if(newMessageFlag == 1 && RX_BUFFER[0] == '{' && RX_BUFFER[1] == 't' && RX_BUFFER[2] == 'i' && RX_BUFFER[3] == 'm' && RX_BUFFER[4] == 'e' && RX_BUFFER[5] == ':')
			  {
	        	  RTC_TimeTypeDef new_time = {0};
	        	  testlen = (RX_BUFFER[6] - '0') * 10 + (RX_BUFFER[7] - '0');
	        	  new_time.Hours = (RX_BUFFER[6] - '0') * 10 + (RX_BUFFER[7] - '0');
	        	  new_time.Minutes = (RX_BUFFER[9] - '0') * 10 + (RX_BUFFER[10] - '0');
	        	  new_time.Seconds = (RX_BUFFER[12] - '0') * 10 + (RX_BUFFER[13] - '0');
	        	  HAL_RTC_SetTime(&hrtc, &new_time, RTC_FORMAT_BIN);
	        	  RTC_DateTypeDef today;
	        	  today.Year = (RX_BUFFER[21] - '0') * 10 + (RX_BUFFER[22] - '0');
	        	  today.Month = (RX_BUFFER[18] - '0') * 10 + (RX_BUFFER[19] - '0');
	        	  today.Date = (RX_BUFFER[15] - '0') * 10 + (RX_BUFFER[16] - '0');
	        	  today.WeekDay = RTC_WEEKDAY_TUESDAY;
	        	  HAL_RTC_SetDate(&hrtc, &today, RTC_FORMAT_BIN);
	        	  newMessageFlag = 0;		//ensures that the code executed by bluetooth message is executed only once
			  }

	          //WRITING START
	          if(newMessageFlag == 1 && RX_BUFFER[0] == '{' && RX_BUFFER[1] == 'w' && RX_BUFFER[2] == 'O' && RX_BUFFER[3] == 'n' && RX_BUFFER[4] == ':')
			  {
	        	  readFlag = 0;
	        	  writeFlag = 1;

				  i = 0;
				  while( RX_BUFFER[i]!='}'){
					  i++;
				  }
				  nameSpaceCounter = i - 5;
				  memcpy(writeArrayDonor, RX_BUFFER + 5, MAX_NAME_LENGTH);	//copying the received buffer without beginning of packet "{wOn:"
				  writeArrayDonor[nameSpaceCounter]=' ';	//getting rid of closing "}" in the packet
				  memcpy(writeArray1, writeArrayDonor, subArraySize);
				  memcpy(writeArray2, writeArrayDonor + subArraySize, subArraySize);
				  memcpy(writeArray3, writeArrayDonor + 2 * subArraySize, subArraySize);
				  newMessageFlag = 0;		//ensures that the code executed by bluetooth message is executed only once
			  }

	          //WRITING STOP
	          if(newMessageFlag == 1 && RX_BUFFER[0] == '{' && RX_BUFFER[1] == 'w' && RX_BUFFER[2] == 'r' && RX_BUFFER[3] == 'i' && RX_BUFFER[4] == 't' && RX_BUFFER[5] == 'e' && RX_BUFFER[6] == 'O' && RX_BUFFER[7] == 'f' && RX_BUFFER[8] == 'f' && RX_BUFFER[9] == '}')
	          {
	        	  writeFlag = 0;
	        	  newMessageFlag = 0;		//ensures that the code executed by bluetooth message is executed only once
	          }

	          //HISTORY OUTPUT
	          if(newMessageFlag == 1 && RX_BUFFER[0] == '{' && RX_BUFFER[1] == 'h' && RX_BUFFER[2] == 'i' && RX_BUFFER[3] == 's' && RX_BUFFER[4] == 't' && RX_BUFFER[5] == 'o' && RX_BUFFER[6] == 'r' && RX_BUFFER[7] == 'y' && RX_BUFFER[8] == '}')
			  {
	        	  /*char buffer1[] = "hist:";
	        	  HAL_UART_Transmit(&huart1, (void*)buffer1, strlen(buffer1), 1000);
	        	  for (int v = 0; v < historyEntranceCounter; v++) {
	        		  HAL_UART_Transmit(&huart1, (void*)myHistory[v].names, strlen(myHistory[v].names), 1000);
	        		  HAL_UART_Transmit(&huart1, (void*)myHistory[v].timeStamps, strlen(myHistory[v].timeStamps), 1000);
	        	  }
	        	  char buffer2[] = "\n";
	        	  HAL_UART_Transmit(&huart1, (void*)buffer2, strlen(buffer2), 1000);
	        	  */


	        	  //sendEncryptedData("HistoryStart");

	        	  sendEncryptedData("HistoryStart");
	        	  HAL_Delay(50);
	        	  for (int v = 0; v < historyEntranceCounter; v++) {
	        	      char buffer[MAX_NAME_LENGTH + TIMESTAMP_LENGTH + 1]; // +1 for null terminator

	        	      // Concatenate names and timeStamps into the buffer
	        	      snprintf(buffer, sizeof(buffer), "h:%s%s", myHistory[v].names, myHistory[v].timeStamps);

	        	      // Transmit the concatenated string
	        	      sendEncryptedData(buffer);
	        	  }
	        	  sendEncryptedData("HistoryEnd");








				  	  newMessageFlag = 0;		//ensures that the code executed by bluetooth message is executed only once
			  }

	          //PASSWORD CHECKING
              //BUT NOT ONLY WE RECEIVE THE PUBLIC KEY, WE ALSO SEND BACK OUT STM PUBLIC KEY, WE EXCHANGE
	          if(newMessageFlag == 1 && RX_BUFFER[0] == '{' && RX_BUFFER[1] == 'p' && RX_BUFFER[2] == 'a' && RX_BUFFER[3] == 's' && RX_BUFFER[4] == 's' && RX_BUFFER[5] == 'w' && RX_BUFFER[6] == 'o' && RX_BUFFER[7] == 'r' && RX_BUFFER[8] == 'd' && RX_BUFFER[9] == ':')
	          {
	        	  if(RX_BUFFER[10] == '1' && RX_BUFFER[11] == '1' && RX_BUFFER[12] == '1'){
	        		  char buffer2[50];
	        		  sprintf(buffer2, "pass_correct,read:%d,write:%d\n",readFlag,writeFlag);
	        		  HAL_UART_Transmit(&huart1, (void*)buffer2, strlen(buffer2), 1000);
	        	  }else{
	        		  char buffer2[] = "pass_incorrect\n";
	        		  HAL_UART_Transmit(&huart1, (void*)buffer2, strlen(buffer2), 1000);
	        	  }
	        	  newMessageFlag = 0;		//ensures that the code executed by bluetooth message is executed only once
	          }

	          //THIS if handles recieved message with the components of a public key received from android application - modulus and exponent
	          if(newMessageFlag == 1 && RX_BUFFER[0] == '{' && RX_BUFFER[1] == 'e' && RX_BUFFER[2] == '1' && RX_BUFFER[3] == ':')
	          {
	              char *endptr;

	              // Extract e1 and e2
	              char e1_str[20], e2_str[20];	//we need to devide the number into two because it is often far larger then max capacity of long int so there are errors, so we devide them during transition and combine again here
	              int i = 0;
	              while (RX_BUFFER[i] != 'e') {
	                  i++;
	              }
	              sscanf(RX_BUFFER + i, "e1:%[^e]e2:%[^m]", e1_str, e2_str);	//writing our two parts of exponent into two variables
	              // Calculate multiplier for e2 only if it's not empty
				  long long multiplier_e2 = 1;

				  for (int k = 0; k < strlen(e2_str); k++) {	//computing by how much to multiply second number for it to be right after first one
					  multiplier_e2 *= 10;
				  }
				  if(strlen(e2_str) > 6){	//if second number is absent than we just print the first one alone
					  pubAPP->exponent = numberpe = strtoll(e1_str, &endptr, 10);	//writing only the first part of exponent into our public key structure if there is only one part
				  }else{
					  pubAPP->exponent = numberpe = strtoll(e1_str, &endptr, 10) * multiplier_e2 + strtoll(e2_str, &endptr, 10);	//writing into that structures both parts if second one exists
				  }


	              // Extract m1 and m2
	              char m1_str[20], m2_str[20];
	              i = 0;
	              while (RX_BUFFER[i] != 'm') {
	                  i++;
	              }
	              sscanf(RX_BUFFER + i, "m1:%[^m]m2:%[^}]", m1_str, m2_str);

	              // Calculate multiplier for m2
	              long long multiplier_m2 = 1;
	              for (int j = 0; j < strlen(m2_str); j++) {
	                  multiplier_m2 *= 10;
	              }
	              if(strlen(m2_str) > 6){
	            	  pubAPP->modulus = strtoll(m1_str, &endptr, 10);
	              }else{
	            	  pubAPP->modulus = strtoll(m1_str, &endptr, 10) * multiplier_m2 + strtoll(m2_str, &endptr, 10);
	              }

	              HAL_Delay(100);

	              //BUT NOT ONLY WE RECEIVE THE PUBLIC KEY, WE ALSO SEND BACK OUT STM PUBLIC KEY, WE EXCHANGE
				strcpy(bufferForOutput, "key{e:");
				HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
				sprintf(bufferForOutput, "%lu", pubSTM->exponent);
				HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
				strcpy(bufferForOutput, "m:");
				HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);
				sprintf(bufferForOutput, "%lu}\n", pubSTM->modulus);
				HAL_UART_Transmit(&huart1, (void*)bufferForOutput, strlen(bufferForOutput), 1000);




	              newMessageFlag = 0;
	          }

	          //THIS if handles all incoming encrypted messages and decrypts them
	          if(newMessageFlag == 1 && RX_BUFFER[0] == 'm' && RX_BUFFER[1] == ':')
	          {

	        	  i = 0;
				  while( RX_BUFFER[i]!='}'){
					  i++;
				  }
				  nameSpaceCounter = i - 2;

				  uint8_t encodedMessageBuffer[nameSpaceCounter]; //here i write incoming encoded messages
				  memcpy(encodedMessageBuffer, RX_BUFFER + 2, nameSpaceCounter);	//copying the received buffer without beginning of packet "{wOn:"
				  //encodedMessageBuffer[nameSpaceCounter]=' ';	//getting rid of closing "}" in the packet


				  size_t arraySize = strlen(encodedMessageBuffer) / 10;


				  size_t numElements;
				  long long *encryptedMessage = extractNumbers(RX_BUFFER, &numElements);

				  decryptedMessage = rsa_decrypt(encryptedMessage, 24*sizeof(encryptedMessage), privSTM);

				  for(int i=0; i<20; i++){
					  testBigBuffer[i]=decryptedMessage[i];
				  }


				  testlen = strncmp("readOn", decryptedMessage, 6);
				  if(strncmp("openDoor", decryptedMessage, 8) == 0){
					  sendEncryptedData("opening door");
					  HAL_GPIO_WritePin(GPIOB, Bluetooth_LED_Pin, GPIO_PIN_SET);
					  HAL_Delay(1000);
					  HAL_GPIO_WritePin(GPIOB, Bluetooth_LED_Pin, GPIO_PIN_RESET);
				  }else if(strncmp("readOn", decryptedMessage, 6) == 0){
					  sendEncryptedData("starting reading");
					  readFlag = 1;
					  writeFlag = 0;
				  }else if(strncmp("readOff", decryptedMessage, 7) == 0){
					  sendEncryptedData("stopping reading");
					  readFlag = 0;
				  }else if(strncmp("syncUser", decryptedMessage, 8) == 0){
					  char buffer1[] = "usr:";
					  HAL_UART_Transmit(&huart1, (void*)buffer1, strlen(buffer1), 1000);
					  for(i = 0; i < myNames.count; i++){
						  HAL_UART_Transmit(&huart1, (void*)myNames.names[i], strlen(myNames.names[i]), 1000);
						  char buffer[] = " ";
						  HAL_UART_Transmit(&huart1, (void*)buffer, strlen(buffer), 1000);
					  }
					  char buffer2[] = "\n";
					  HAL_UART_Transmit(&huart1, (void*)buffer2, strlen(buffer2), 1000);



					  sendEncryptedData("UsersStart");
					  HAL_Delay(50);
					  for(i = 0; i < myNames.count; i++){
						  char buffer[MAX_NAME_LENGTH + TIMESTAMP_LENGTH + 1]; // +1 for null terminator

						  // Concatenate names and timeStamps into the buffer
						  snprintf(buffer, sizeof(buffer), "u:%s ", myNames.names[i]);

						  // Transmit the concatenated string
						  sendEncryptedData(buffer);
					  }
					  HAL_Delay(50);
					  sendEncryptedData("UsersEnd");
				  }else if(strncmp("history", decryptedMessage, 7) == 0){
					  sendEncryptedData("HistoryStart");
					  HAL_Delay(50);
					  for (int v = 0; v < historyEntranceCounter; v++) {
						  char buffer[MAX_NAME_LENGTH + TIMESTAMP_LENGTH + 1]; // +1 for null terminator

						  // Concatenate names and timeStamps into the buffer
						  snprintf(buffer, sizeof(buffer), "h:%s%s", myHistory[v].names, myHistory[v].timeStamps);

						  // Transmit the concatenated string
						  sendEncryptedData(buffer);
					  }
					  HAL_Delay(50);
					  sendEncryptedData("HistoryEnd");
				  }else if(strncmp("add:", decryptedMessage, 4) == 0){

					  i = 0;
					  while( decryptedMessage[i]!='}'){
						  i++;
					  }
					  nameSpaceCounter = i - 4;
					  addName(&myNames, decryptedMessage, 4, nameSpaceCounter);	//nameSpaceCounter is length of the name and 5 is index where the name starts in buffer

				  }else if(strncmp("Dlt:", decryptedMessage, 4) == 0){
					  i = 0;
					  while( decryptedMessage[i]!='}'){
						  i++;
					  }
					  nameSpaceCounter = i - 4;
					  uint8_t nameToDelete[nameSpaceCounter + 1];  // +1 for null-terminator
					  memcpy(nameToDelete, decryptedMessage + 4, nameSpaceCounter);
					  nameToDelete[nameSpaceCounter] = '\0';  // Null-terminate the string
					  deleteName(&myNames, nameToDelete);
				  }else if(strncmp("writeOff", decryptedMessage, 8) == 0){
					  writeFlag = 0;
				  }else if(strncmp("wOn:", decryptedMessage, 4) == 0){
					  readFlag = 0;
					  writeFlag = 1;

					  i = 0;
					  while( decryptedMessage[i]!='}'){
						  i++;
					  }
					  nameSpaceCounter = i - 4;
					  memcpy(writeArrayDonor, decryptedMessage + 4, MAX_NAME_LENGTH);	//copying the received buffer without beginning of packet "{wOn:"
					  writeArrayDonor[nameSpaceCounter]=' ';	//getting rid of closing "}" in the packet
					  memcpy(writeArray1, writeArrayDonor, subArraySize);
					  memcpy(writeArray2, writeArrayDonor + subArraySize, subArraySize);
					  memcpy(writeArray3, writeArrayDonor + 2 * subArraySize, subArraySize);
				  }else if(strncmp("pass:", decryptedMessage, 5) == 0){
					  if(decryptedMessage[5] == '1' && decryptedMessage[6] == '1' && decryptedMessage[7] == '1'){
						  char buffer2[50];
						  sprintf(buffer2, "pass_correct,read:%d,write:%d",readFlag,writeFlag);
						  sendEncryptedData(buffer2);
					  }else{
						  char buffer2[] = "pass_incorrect";
						  sendEncryptedData(buffer2);
					  }
				  }









				  free(encryptedMessage);
				  free(decryptedMessage);



	        	  newMessageFlag = 0;
	          }




    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x3;
  sTime.Minutes = 0x23;
  sTime.Seconds = 0x30;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_OCTOBER;
  sDate.Date = 0x9;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 720;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|RFID_WRITE_LED_Pin|Bluetooth_LED_Pin|RFID_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 RFID_WRITE_LED_Pin Bluetooth_LED_Pin RFID_LED_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|RFID_WRITE_LED_Pin|Bluetooth_LED_Pin|RFID_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
