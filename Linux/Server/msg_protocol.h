#ifndef MSG_PROTOCOL_H
#define MSG_PROTOCOL_H

#include <stdint.h>

#define MSG_TYPE_OI 0    /**< Mensagem de identificação incial */
#define MSG_TYPE_TCHAU 1 /**< Mensagem de despedida */
#define MSG_TYPE_MSG 2   /**< Mensagem de texto */
#define MSG_TYPE_ERRO 3  /**< Mensagem de erro */

/**
 * @brief Estrutura que representa uma mensagem no protocolo.
 */
typedef struct {
  uint16_t type;     /**< Tipo da mensagem */
  uint16_t orig_uid; /**< Identificador do usuário de origem */
  uint16_t dest_uid; /**< Identificador do usuário de destino */
  uint16_t text_len; /**< Tamanho do texto */
  uint8_t text[141]; /**< Texto da mensagem */
} msg_t;

#endif  // MSG_PROTOCOL_H
