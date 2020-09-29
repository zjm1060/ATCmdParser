/**
 ******************************************************************************
 * @file    ATCmdParser.h
 ******************************************************************************
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

#ifndef _AT_CMD_PARSER_H_
#define _AT_CMD_PARSER_H_

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup emhost */
/** @{*/
#define AT_BUFFER_SIZE	(2048)
/** \addtogroup AT_parser */
/** @{*/

/******************************************************************************
 *                                 Constants
 ******************************************************************************/

/******************************************************************************
 *                               Type Definitions
 ******************************************************************************/

/**
 * Incomming AT out-of-band packet handler
 */
typedef void (*oob_callback)(void *);

/**
 * Incomming AT out-of-band packet format link node
 */
struct oob {
    unsigned len;
    const char* prefix;
    oob_callback cb;
    void* next;
};

/******************************************************************************
 *                             Function Declarations
 ******************************************************************************/
typedef struct{
	int (*get)(int);
	int (*put)(char);
	int (*readable)();
	int (*init)(int);
}serial_ops;

typedef struct{
	serial_ops *ops;
	struct oob* _oobs;
	void (*unprocessed_data)(const char *,int );
	int character_timeout;
	bool _dbg_on;
	const char* _output_delimiter;
	int _output_delim_size;
	const char* _input_delimiter;
	int _input_delim_size;
	char _buffer[AT_BUFFER_SIZE];
}ATParser;

/**
 * @brief Enable debug mode
 *
 * @return none
 */
void ATCmdParser_debug(ATParser *at, bool on);

/**
 * @brief 		AT command parser initialize
 * 
 * @param[in] 	output_delimiter: AT output command delimiter chars, "\r", "\r\n", "\n", etc
 * @param[in] 	input_delimiter: AT input respond and event delimiter chars, "\r", "\r\n", "\n", etc
 * @param[in] 	timeout: AT receive timeout, also changable by  #ATCmdParser_set_timeout
 * @param[in]	debug: Enable debug mode or not
 *
 * @return none
 */
ATParser *ATCmdParser_init(serial_ops *hal, const char* output_delimiter, const char* input_delimiter, int timeout, bool debug);

/**
 * @brief 			Recv AT command respont, and parse AT parameters to variables
 * 
 * @param[in] 		Incomming respond format, refer to printf
 *
 * @return 			true: Success, false: Timeout or format not match
 */
bool ATCmdParser_recv(ATParser *at, const char* response, ...);

/**
 * @brief 			Send AT command
 * 
 * @param[in] 		AT command format, refer to scanf
 *
 * @return 			true: Success, false: Serial port send error
 */
bool ATCmdParser_send(ATParser *at, const char* command, ...);

/**
 * @brief 			Add a handler to an incomming out-of-band packet type, 
 *                  example: oob format: "\r\n+<TYPE>：[para-1,para-2,para-2,...,para-n]\r\n", 
 *                  "+<TYPE>" is the prefix
 * @note    		Never send a AT command in the handler
 * 
 * @param[in] 		prefix: incomming oob packet prefix.
 *
 * @return 			none
 */
void ATCmdParser_add_oob(ATParser *at, const char* prefix, oob_callback cb);

/**
 * @brief 			Read raw data from AT command serial port
 * 
 * @param[out] 		data: Buffer to store the incomming data
 * @param[in] 		size: Buffer size
 * 
 * @return 			size of the actually received data
 */
int ATCmdParser_read(ATParser *at, char* data, int size);

/**
 * @brief 			Send raw data to AT command serial port
 * 
 * @param[in] 		data: Point to the data buffer ready to send
 *@param[in] 		size: Buffer size
 * 
 * @return 			size of the actually sent data
 */
int ATCmdParser_write(ATParser *at, const char* data, int size);

/**
 * @brief 			Set AT parser timeout
 * 
 * @param[in] 		timeout: new timeout value
 *
 * @return 			none
 */
void ATCmdParser_set_timeout(ATParser *at, int timeout);

/**
 * @brief 			Receive and parse incomming out-of-band packet
 *
 * @return 			true: proccessed a packet, false: no packet to process
 */
bool ATCmdParser_process_oob(ATParser *at);

/**
 * @brief 			Analyse string parameters form AT command respond, 
 *                  Respond format: "[\r\n][+CMD:][para-1,para-2,para-3,......]<\r\n><STATUS><\r\n>"
 *                  First, read para list "para-1,para-2,para-3,......" from respond to string by #ATCmdParser_recv
 *                  second, use this function to parse every string para into an array
 *                  example: "111,222,333\,33,444" to "111", "222", "333,33", "444"
 *
 * @param[in] 		args: parameters string, like para-1,para-2,para-3,......
 * @param[out] 		arg_list: point to parameters list array
 * @param[in] 		list_size: parameters list array size
 * 
 * @return 			true: proccessed a packet, false: no packet to process
 */
int ATCmdParser_analyse_args(ATParser *at, char args[], char* arg_list[], int list_size);


void ATCmdParser_set_unprocessed_cb(ATParser *at, void (*cb)(const char *,int ));
/** @}*/
/** @}*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //_AT_CMD_PARSER_H_
