# SimpleLight
Простая подсветка с управлением через web на базе ESP8266

Возможности и фичи:
* Веб конфигуратор
* Колесо выбора цвета
* Прошивка по воздуху
* Конфигурация для подключения к роутеру
* Динамический выбор количества светодиодов

Режимы:
* Белый цвет
* Цвет на выбор
* Плавная смена цвета
* Радуга
* Огонь с выбором цвета

Остальное:
* Запуск точки доступа, если нет подключения к роутеру
* Сохранение всех настроек в EEPROM
* MDNS, LLMNR, NetBios - для подключения по доменному имени, а не ip, пока только на windows и apple

### Сборка
Сборка максимально проста, всё что вам нужно:
* Nodemcu v3 lolin
* Конденсатор 6.3v 1000µF
* Любой резистор на 200-500Ω
* Паяльник
* Прямые руки (опционально)

![Scheme](https://raw.githubusercontent.com/NikoVonLas/simpleLight/master/README/scheme.png)
Пин Din ленты обязательно подключать к Rx пину, ибо используемая библиотека игнорирует другие, уж не знаю почему.

### Прошивка
Есть два варианта прошивки: из готовых и скомпилированных файлов и их исходников.
#### Из скомпилированных .bin файлов
### NodeMCU Flasher
~~Качаем последнюю версию [NodeMCU Flasher](https://github.com/nodemcu/nodemcu-flasher), для вашей платформы) и загружаем прошивку с данными настройками:~~
![Scheme](https://raw.githubusercontent.com/NikoVonLas/simpleLight/master/README/flasher.png)

По какой-то причини после прошивки таким образом устройство отказывается работать, разбираюсь в чём причина.
#### Из исходников
Скопировать библиотеки из папки src\libraries в рабочую папку arduino ide (обычно Документы\Arduino\libraries).

Установить ядро для ESP8266. В arduino ide: Файл -> Настройки, и в поле "Дополнительные ссылки для менеджера плат" вбить http://arduino.esp8266.com/stable/package_esp8266com_index.json

Далее в arduino ide: Инструменты -> Плата -> Менеджер плат, находим "esp8266 by ESP8266 Community" и устанавливаем последнюю версию.

Подменяем файлы ядра из папки src\core в рабочую папку arduino ide (Документы\ArduinoData\packages\esp8266\hardware\esp8266\2.5.2\cores\esp8266).

Переносим файлы из src\sketch в рабочую папку arduino ide(Документы\Arduino).

Открыть скетч из папки sketch и прошить вашу плату.
Версия arduino ide: 1.8.9 (Windows store 1.8.21.0)
Настройик платы(для NodeMCU v3 Loin):
```
Плата: NodeMCU 1.0 (ESP12E Module)
Upload speed: 115200
CPU Frequency: 160MHz
Flash size: 4MB (FS:2MB OTA:~1019KB)
Debug port: Disabled
Debug level: Ничего
IwIP Variant: v2 Higher Bandwidth
VTables: Flash
Exceptions: Disabled
Erase Flash: Only Sketch
SSL Support: All SSL ciphers
```

Далее нужно установить [плагин](https://github.com/esp8266/arduino-esp8266fs-plugin) для arduino ide, чтоб было возможно загружать файлы. Установили - перезапускайте ide и заливайте  файлы(Инструменты -> ESP8266 Sketch data upload). Если выдаёт ошибку, попробуйте переподключить плату и сразу(!) как только подключили снова нажать нужный пункт меню.

Вот, в общем-то и всё. Должна появится новая точка доступа "SimpleLight":
```
SSID: SimpleLight
Pass: SimpleLightPassword
IP: 192.168.42.1
Host: simplelight.local
```
#### Изменение исходников
В папке src\sketch\assets_src лежат все необходимые файлы(css, js, img) для работы web конфигуратора. Если будете их менять - либо собирайте их при помощи NPM, либо в src\sketch\data\www\index.html пропишите их отдельное подключение.

Если собираете при помощи NPM - откройте консоль в папке src\sketch, и предварительно установив [NPM](https://www.npmjs.com/get-npm) пропишите:
```
npm install
gulp css
gulp js
```
Минимизированные файлы автоматически добавятся в папку src\sketch\data\assets.

#### Обновление через веб конфигуратор
Имя файла для SPIFFS обязательно должно быть spiffs.bin, для прошивки FLASH - имя файла не важно.

### ToDo

 - Добавление автопроверки обновления и скачивание его с гитхаба
 - Добавление режимов
 - Подключение по доменному имени для android
 - Возможность подключения фоторезистора для автоматической регулировки
 - Интерфейс для работы с голосовыми ассистентами
 - Возможность менять пароль для точки доступа в вебконфигураторе

Лицензия/License
----
MIT
[Русский](https://github.com/NikoVonLas/simpleLight/blob/master/LICENSE)
[English](https://github.com/NikoVonLas/simpleLight/blob/master/LICENSE_EN)
