//1.3 Disable force video-mode change on video startup because many display is indicate when mode changed
//1.2 Disable oxplayer is logging to screen when start playing video. see line 168
//1.1 Force HDMI sound out
//	  Open file 1.mp4 from home/pi/Videos
//1.0 First stable
//ERROR
//1001 get s/n file error
//1002S BAD SN
//2001 GET MAC FILE ERROR
//2002 MEMORY ERROR WHILE MAC CHECK
//2003M BAD MAC

//PINOUT
//+-------+-----+---------+------+---+---Pi 2---+---+------+---------+-------+
//| FEAR  | wPi |   Name  | Mode | V | Physical | V | Mode | Name    |  FEAR |
//+-------+-----+---------+------+---+----++----+---+------+---------+-------+
//|       |     |    3.3v |      |   |  1 || 2  |   |      | 5v      |       |
//|       |   8 |   SDA.1 |   IN | 1 |  3 || 4  |   |      | 5V      |PIR_VCC|
//|       |   9 |   SCL.1 |   IN | 1 |  5 || 6  |   |      | 0v      |PIR GND|
//|LED_OUT|   7 | GPIO. 7 |  OUT | 0 |  7 || 8  | 1 | ALT0 | TxD     |       |
//|LED_GND|     |      0v |      |   |  9 || 10 | 1 | ALT0 | RxD     |       |
//|PIR_IN |   0 | GPIO. 0 |   IN | 0 | 11 || 12 | 0 | IN   | GPIO. 1 |       |
//+-------+-----+---------+------+---+---Pi 2---+---+------+---------+-------+
//| FEAR  | wPi |   Name  | Mode | V | Physical | V | Mode | Name    |  FEAR |
//+-------+-----+---------+------+---+----++----+---+------+---------+-------+
//-------------------------------------------------------------------------

#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
//-----------------------------------------------------------------------
//
// Remove leading and trailing whitespace from a string.

static char *
trimWhiteSpace(
    char *string)
{
    if (string == NULL)
    {
        return NULL;
    }

    while (isspace(*string))
    {
        string++;
    }

    if (*string == '\0')
    {
        return string;
    }

    char *end = string;

    while (*end)
    {
        ++end;
    }
    --end;

    while ((end > string) && isspace(*end))
    {
        end--;
    }

    *(end + 1) = 0;
    return string;
}

//-----------------------------------------------------------------------

int main(void)
{//SERIAL NUMBER CHECK

	uint32_t serial = 0;

    FILE *fp = fopen("/proc/cpuinfo", "r");

    if (fp == NULL)
    {
        perror("1001");
        exit(EXIT_FAILURE);
    }

    char entry[80];

    while (fgets(entry, sizeof(entry), fp) != NULL)
    {
        char* saveptr = NULL;

        char *key = trimWhiteSpace(strtok_r(entry, ":", &saveptr));
        char *value = trimWhiteSpace(strtok_r(NULL, ":", &saveptr));

        if (strcasecmp("Serial", key) == 0)
        {
            serial = strtoul(value, NULL, 16);
        }
    }

    fclose(fp);

    if (serial != 2180177200)
    {
        perror("1002S");
        exit(EXIT_FAILURE);
    }


//MAC ADRESS CHECK
  FILE * pFile;
  long lSize;
  char * buffer;
  size_t result;

  pFile = fopen ( "/sys/class/net/eth0/address", "rb" );
  if (pFile==NULL)
  	  {
	  perror("2001");
	  exit(EXIT_FAILURE);
	  }

  // obtain file size:
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);

  // allocate memory to contain the whole file:
  buffer = (char*) malloc (sizeof(char)*lSize);
  if (buffer == NULL) {
	  perror("2002");
	  exit(EXIT_FAILURE);
	  }



  // copy the file into the buffer:
  result = fread (buffer,1,lSize,pFile);
//  if (result != lSize) {fputs ("Reading error",stderr); exit (3);}



  int x = strcmp(buffer, "b8:27:eb:f2:dd:30\n");
  if (x != 0){
	  perror("2003M");
	  exit(EXIT_FAILURE);
	  }

  // terminate
  fclose (pFile);
  free (buffer);


  	  int pin_out = 7;
  	  int pin_in = 0;

  	  if (wiringPiSetup() == -1){
  		  perror("3000");
  			  exit(EXIT_FAILURE);
  	  }


  	  system("clear");

  	  pinMode(pin_out, OUTPUT);
  	  pinMode(pin_in, INPUT);
  	  pullUpDnControl (pin_in,PUD_DOWN);

  	  for (;;){
  	  if (digitalRead (pin_in)) {
  		//  printf("LED On\n");
  		digitalWrite(pin_out, 1);

  		//system("omxplayer -b -r -o hdmi /home/pi/Videos/1.mp4| echo""");					//Проигрываем первый ролик
  		system("omxplayer -b -o hdmi /home/pi/Videos/1.mp4| echo""");					//Проигрываем первый ролик
  		system("clear");
  		digitalWrite(pin_out, 0);//Проигрываем первый ролик
  		//  system("omxplayer -b /home/pi/2.mp4");					//Проигрываем второй ролик
  		//  delay(250);
  		while (digitalRead (pin_in)){};
  	  }

  	  }
    return 0;
}
