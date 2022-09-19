# Watermeter Bluetooth LE Telink TLSR8253 (Ai-Thinker TB-04)

_Сразу хочу выразить благодарность Виктору pvvx за его работу [ATC_MiThermomete](https://github.com/pvvx/ATC_MiThermometer). Это мне помогло разобраться в нюансах программирования BLE на TLSR8253 (хотя не факт, что я написал полностью правильный код :)). Ну и еще он отвечал на мои глупые вопросы на [форуме](https://esp8266.ru/forum/threads/ble-moduli-tb-04-tb-03f-tlsr8253f512.5362). Спасибо._

[Repository watermeter_ble](https://github.com/slacky1965/watermeter_ble)

**Описание**

* Рассчитано на два счетчика воды.
* Не работает с системой namur, только замыкание-размыкание (сухой контакт).
* Ведет подсчет замыканий-размыканий, увеличивая каждый раз количество литров на заданное значение от 1 до 10 литров (по умолчанию 10 литров на один импульс).
* Сохраняет показания в энергонезависимой памяти модуля.
* Передает показания по Bluetooth LE в рекламном незашифрованном пакете в формате [BTHome](https://bthome.io/).
* Первоначальная настройка происходит через [WEB-интерфейс](https://slacky1965.github.io/watermeter_ble_utils/TelinkFlasherAndConfig.html). 
* Соединиться с модулем можно только с того устройства, с которого производили первое соединение. Первый раз соединиться можно с любого устройства.

**Железо**

Испытывалось все на вот таком модуле от Ai-Thinker TB-04-KIT

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/images/TB-04-KIT-top.jpg" alt="Ai-Thinker TB-04-KIT"/>

Чтобы в этот модуль залить прошивку нужно 2-ю ножку микросхемы CH340 соединить с выводом SWS.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/images/TB-04-KIT-wire.jpg" alt="Ai-Thinker TB-04-KIT"/>

Саму программу заливки можно скачать у pvvx - [TlsrComProg](https://github.com/pvvx/TlsrComProg825x). Спасибо ему еще раз.
По большому счету нужны всего два файла **TlsrComProg.py** и **floader.bin**

**Корпус**

Решение самодостаточного модуля было подсмотрено в проекте [waterius](https://github.com/dontsovcmc/waterius).
Чтобы не изобретать, куда это потом все прятать, берем, так называемый, Battery Holder Box на 3 батарейки АА. Переделываем на 2 батарейки и получаем питание 3 вольта и один сегмент под плату.
<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/box/box1.jpg" alt="BOX 3AA"/>
<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/box/box2.jpg" alt="BOX 3AA"/>
<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/box/box3.jpg" alt="BOX 3AA"/>
<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/box/box4.jpg" alt="BOX 3AA"/>

**Плата**

[Схема и плата](https://oshwlab.com/slacky/watermeter_tlsr8253).

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/board/board_top.jpg" alt="PCB"/>

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/board/module.jpg" alt="PCB"/>

**Софт**

[Прошивку](https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/watermeter_ble_V1.6.bin) нужно залить в модуль с помощью [github.com/pvvx/TLSRPGM](https://github.com/pvvx/TLSRPGM) или [github.com/pvvx/TlsrComProg825x](https://github.com/pvvx/TlsrComProg825x). Это уж как кому нравится. Затем соедиться с ним через [Web-интерфейс](https://slacky1965.github.io/watermeter_ble_utils/TelinkFlasherAndConfig.html) и настроить начальные данные - число литров горячей воды, число литров холодной воды, а также количество литров на один импульс (смотрите документацию на ваш счетчик). А также обновить прошивку "по воздуху", если нужно.
<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/images/connect.jpg" alt="Web flasher"/>

Первое соединение происходит достаточно быстро, потому что период обнаружения выбран малый, 250 мсек. После первого соединение этот период увеличавется до 2.5 секунд для экономии батарейки. Потому на последующие соединения, если таковые будут нужны, понадобится больше времени (не пугайтесь, все работает).

Вроде все. Будут вопросы, задавайте, если смогу, отвечу.

**P.S.**

Забыл сказать. BTHome (или бывший ble_monitor) пока не умеет правильно обрабатывать пакет, в котором имеются несколько значений одного типа. Но это обещали поправить в самом ближайшем будущем - [github.com/custom-components/ble_monitor/issues/854](https://github.com/custom-components/ble_monitor/issues/854)

