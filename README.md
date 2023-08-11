## Обозначения

## Catalog1
    базовые операции
    статическая, динамическая память
    библеотеки: статические, динамические, подгружаемые
    файловая система

## Catalog2
    процессы
    потоки
    Неименованные и именованные каналы
    Очередь сообщений
    Разделяемая память и семафоры
    Сигналы

## D23: Сокеты
    DD1: AF_LOCAL SOCK_STREAM
    DD2: AF_LOCAL SOCK_DGRAM
    DD3: AF_INET SOCK_STREAM
    DD4: AF_INET SOCK_DGRAM


## D24: Схемы серверов


    clients
        thread_clients.c - проверка и нагрузка сервера
        Аргументы(последние 4 необязательные):
            адрес сервера
            порт сервера
            количество подключаемых клиентов(1 поток - 1 клиент)
            состояния одновременного подключения
            состояние генерация случайных параметров(если 1), в пределах последующих аргументов
            количество сообщений
            количество секунд до подключения(если одновременное подключение, то после того как все клиенты будут инициализированы)
            интервал между сообщениями
        Пример команды запуска: ./thread_clients 127.0.0.1 20003 100 0 0 10 1 1
    scheme_1
        Динамическое создание обслуживающих потоков слущающим сервером
        Аргументы запуска: адресс сервера, порт сервера 
    scheme_2
        Заранее задан пул облужиающих потоков
        Если весь пулл использован, дополнительно создаются еще потоки
        Аргументы запуска: адресс сервера, порт сервера, размер пула
    scheme_3
        Реализация очереди
        Слущающий сервер записывает соединения в канал
        Дополнительный поток для создания создания соединений
        Аргументы запуска: адресс сервера, порт сервера 
    scheme_4
        Используются poll и epoll
        Слущающий сервер записывает соединения в канал
        Потоки считывают соединения для обработки
        Каждый поток может обрабатывать N заранее заданных соединений


## D25: RAW сокеты 
    заполнение UDP заголовков
    
## D26: RAW сокеты
    заполнение IP заголовка

## D27: ethernet header
    формирование заголовков:
        udp
        ip
        ethernet
    ./server <адресс> <порт>
    ./server_v2 - определен статически в коде, другой способ завершения работы
    ./client <имя сетевого устройства> <mac источника> <mac сервера> <ip источника> <ip сервера> <порт источника> <порт сервера>
    Пример:
        sudo ./client enp0s3 08:00:27:10:07:3b 08:00:27:14:8c:3c 192.168.56.101 192.168.56.102 54321 4567
        /server 192.168.56.102 4567


## D28
    Broadcast
    Multicast















