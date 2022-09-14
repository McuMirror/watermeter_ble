# Watermeter Bluetooth LE Telink TLSR8253 (Ai-Thinker TB-04)

_Сразу хочу выразить благодарность Виктору pvvx за его работу [ATC_MiThermomete](https://github.com/pvvx/ATC_MiThermometer). Это мне помогло разобраться в нюансах программирования BLE (хотя не факт, что я написал полностью правильный код :)). Ну и еще он отвечал на мои глупые вопросы на [форуме](https://esp8266.ru/forum/threads/ble-moduli-tb-04-tb-03f-tlsr8253f512.5362). Спасибо._

**Описание**
---
* Рассчитано на два счетчика воды.
* Не работает с системой namur, только замыкание-размыкание (сухой контакт).
* Ведет подсчет замыканий-размыканий, увеличивая каждый раз количество литров на заданное значение от 1 до 10 литров (по умолчанию 10 литров на один импульс).
* Сохраняет показания в энергонезависимой памяти модуля.
* Передает показания по Bluetooth LE в рекламном незашифрованном пакете в формате [BTHome](https://bthome.io/).
* Первоначальная настройка происходит через [WEB-интерфейс](https://www.slacky.ru/watermeter_ble/TelinkFlasherAndConfig.html). 
* Соединиться с модулем можно только с того устройства, с которого производили первое соединение. Первый раз соединиться можно с любого устройства.

**Железо**
---
Испытывалось все на вот такой плате
<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/images/TB-04-KIT-top.jpg" alt="Ai-Thinker TB-04-KIT"/>

Чтобы в этот модуль залить прошивку нужно 2 ножку микросхемы CH340 замкнуть на SWS.
<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/images/TB-04-KIT-wire.jpg" alt="Ai-Thinker TB-04-KIT"/>

Саму программу заливки можно скачать у pvvx - [TlsrComProg](https://github.com/pvvx/TlsrComProg825x). Спасибо ему еще раз.
По большому счету нужны всего два файла **TlsrComProg.py** и **floader.bin**

Решение самодостаточного модуля было подсмотрено у [waterius](https://github.com/dontsovcmc/waterius).

Плата.


---

Продолжение следует ...



