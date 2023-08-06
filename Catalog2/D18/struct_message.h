#define SIZE_MSG 50
struct msg_buf{
    long mtype;
//1 - если сообщение содержит никнейм для включения в список
//2 - выход и исключение его из списка
    int init;
    char mtext[SIZE_MSG];
};
#define SIZE_STRUCT_MSG sizeof(struct msg_buf) - sizeof(long)
#define TYPE_INIT 2 // инициализация или выход клиента
#define TYPE_SEND_USER_MSG 10 //сообщение клиента
#define TYPE_SENDING_MSG 9//рассылка сообщения клиента
#define TYPE_MSG_SERVICE 6 // сервисное сообщение
#define TYPE_SENDING_SERVICE_MSG 5//рассылка сервисных сообщений(список пользователей)
#define QUEUEFILENAME "." // файл очереди сообщения