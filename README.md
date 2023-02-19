# Watermeter Bluetooth LE Telink TLSR8253 (Ai-Thinker TB-04)

_Сразу хочу выразить благодарность Виктору pvvx за его работу [ATC_MiThermomete](https://github.com/pvvx/ATC_MiThermometer). Это мне помогло разобраться в нюансах программирования BLE на TLSR8253 (хотя не факт, что я написал полностью правильный код :)). Ну и еще он отвечал на мои глупые вопросы на [форуме](https://esp8266.ru/forum/threads/ble-moduli-tb-04-tb-03f-tlsr8253f512.5362). Спасибо._

_А также моему сыну Алексею, который помог мне со стилями в html, чтобы все выглядело так, как я хотел. Спасибо._

[Repository watermeter_ble](https://github.com/slacky1965/watermeter_ble)

---

**Описание**

* Рассчитано на два счетчика воды.
* Не работает с системой namur, только замыкание-размыкание (сухой контакт).
* Ведет подсчет замыканий-размыканий, увеличивая каждый раз количество литров на заданное значение от 1 до 10 литров (по умолчанию 10 литров на один импульс).
* Сохраняет показания в энергонезависимой памяти модуля.
* Передает показания по Bluetooth LE в рекламном пакете в формате [BTHome V2.0](https://bthome.io/format/).
* Данные датчиков в пакете могут быть зашифрованы [AES-CCM](https://bthome.io/encryption/).
* Первоначальная настройка происходит через [WEB-интерфейс](https://slacky1965.github.io/ble_utils/WatermeterConfig.html). 
* Соединиться с модулем можно только с того устройства, с которого производили первое соединение. Первый раз соединиться можно с любого устройства. Чтобы сбросить whitelist, нужно замкнуть контакт SWS на землю более, чем на 5 секунд (или через [WEB-интерфейс](https://slacky1965.github.io/ble_utils/WatermeterConfig.html), если первоначальное устройство соединения доступно).

---

**Железо**

Выбор пал на недорогой модуль от Ai-Thinker TB-04 (чуть больше 120 рублей)

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/images/TB-04-top.jpg" alt="Ai-Thinker TB-04"/>

Испытывалось все на вот таком модуле от Ai-Thinker TB-04-KIT

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/images/TB-04-KIT-top.jpg" alt="Ai-Thinker TB-04-KIT"/>

Чтобы в этот модуль залить прошивку нужно 2-ю ножку микросхемы CH340 соединить с выводом SWS.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/images/TB-04-KIT-wire.jpg" alt="Ai-Thinker TB-04-KIT"/>

Саму программу заливки можно скачать у pvvx - [TlsrComProg](https://github.com/pvvx/TlsrComProg825x). Спасибо ему еще раз.
По большому счету нужны всего два файла **TlsrComProg.py** и **floader.bin**

---

**Корпус**

Решение самодостаточного модуля было подсмотрено в проекте [waterius](https://github.com/dontsovcmc/waterius).
Чтобы не изобретать, куда это потом все прятать, берем, так называемый, Battery Holder Box на 3 батарейки АА. Переделываем на 2 батарейки и получаем питание 3 вольта и один сегмент под плату.
<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/box/box1.jpg" alt="BOX 3AA"/>
<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/box/box2.jpg" alt="BOX 3AA"/>
<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/box/box3.jpg" alt="BOX 3AA"/>
<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/box/box4.jpg" alt="BOX 3AA"/>

---

**Плата**

В общем есть два варианта. Выбирайте любой. Все работает. Проблема все-таки была не в разводке, а в коде.

[Схема и плата (версия 1)](https://oshwlab.com/slacky/watermeter_tlsr8253)

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/board/board_top_2D.jpg" alt="PCB_V1"/>

[Схема и плата (версия 2)](https://oshwlab.com/slacky/watermeter_ble_v2)

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/board/board_top_2D_V2.jpg" alt="PCB_V2"/>

---

**Готовое устройство**

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/images/watermeter_ble_v2.jpg" alt="PCB_V1"/>

---

**Софт**

[Прошивку](https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/watermeter_ble_V1.9.bin) нужно залить в модуль с помощью [github.com/pvvx/TLSRPGM](https://github.com/pvvx/TLSRPGM) или [github.com/pvvx/TlsrComProg825x](https://github.com/pvvx/TlsrComProg825x). Это уж как кому нравится. Затем соедиться с ним через [Web-интерфейс](https://slacky1965.github.io/ble_utils/WatermeterConfig.html) и настроить начальные данные - количество литров горячей воды, количество литров холодной воды и количество литров на один импульс (смотрите документацию на ваш счетчик). А также обновить прошивку "по воздуху". Если нужно шифрование передаваеммых данных, вводим ключ длиной 16 байт, например '231d39c1d7cc1ab1aee224cd096db932', и отправляем его, нажав на кнопку 'Set BindKey'.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/images/connect.jpg" alt="Web flasher"/>

Первое соединение происходит достаточно быстро, потому что период обнаружения выбран малый, 250 мсек. После первого соединение этот период увеличивается до 2.5 секунд для экономии батарейки. Потому на последующие соединения, если таковые будут нужны, понадобится больше времени (не пугайтесь, все работает).

---

Вроде все. Будут вопросы, задавайте, если смогу, отвечу.

Скриншот из Home Assistant

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/images/homeassistant.jpg" alt="Home Assistant"/>

[Пример автоматизации для Home Assistant](https://github.com/slacky1965/watermeter_ble/tree/main/doc/HomeAssistant/automation)

---

**История версий**

- 1.0 - Начало.
- 1.1 - Добавлена возможность обновления по OTA. 
- 1.2-1.6 - Эксперименты с изменениями версий для конфигурирования через web-интерфейс. Добавления нотификаций.
- 1.7 - Добавлена возможность отображать отладочную информацию с модуля в web-интерфейсе при конфигурации.
- 1.8 - Добавлена возможность сбросить whitelist через замыкание SWS на землю на время более 5 секунд.
- 1.9 - Изменен формат сообщений согласно новой спецификации BTHome V2.0. Добавлена возможность шифровать данные датчиков AES CCM.
- 2.0 - Устранена проблема с подсчетом импульсов.
