# Automation for Home Assistant

Данная автоматизация отправляет время (в виде таймстампа) и значение счетчика при его изменении (увеличении) в mqtt-топик `/watermeter/14_412445/hotwater` вот в таком виде `'1672665759 230'`

В configuration.yaml у вас должно быть что-то типа такого 

`automation: !include_dir_merge_list includes/automation`

Где будут лежать все файлы с автоматизациями. Кладем туда watermeter.yaml. Проверяем конфиг на правильный синтаксис и если все нормально, обновляем автоматизации.

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/images/ha_dt.jpg" alt="Home Assistant Developer Tools"/>

Теперь при срабатывании како-либо датчика, холодной или горячей воды, произойдет автоматическая отправка в выбранный топик MQTT.

Разберем немного более подробней.

```yaml
- alias: Hot Water 14-412445
  trigger:
    - platform: state
      entity_id: sensor.watermeter_ble_c3bd16_count_hot
      not_from:
        - "unknown"
        - "unavailable"
  condition:
    and:
      - condition: template
        value_template: "{{ states('sensor.watermeter_ble_c3bd16_count_hot') | lower != 'unavailable' }}"
      - condition: template
        value_template: "{{ states('sensor.watermeter_ble_c3bd16_count_hot') | lower != 'unknown' }}"
  action:
    - service: mqtt.publish
      data:
        topic: /watermeter/14_412445/hotwater
        payload: "{{ as_timestamp(now()) | int }} {{ states('sensor.watermeter_ble_c3bd16_count_hot') }}"
        
```


`- alias: Hot Water 14-412445` *имя автоматизации, как его будет отображать Home Assistant. 14-412445 - серийный номер счетчика воды.*

**trigger**

`entity_id: sensor.watermeter_ble_c3bd16_count_hot` *id нужного датчика, любое изменение состояния этого сенсора приведет к срабатыванию триггера*

<img src="https://raw.githubusercontent.com/slacky1965/watermeter_ble/main/doc/images/ha_counter_id.jpg" alt="Home Assistant Counter's ID"/>

```yaml
not_from:
        - "unknown"
        - "unavailable"
```
*игнорировать изменения датчика, когда он срабатывает из предыдущего значения unknown или unavailable, чтобы не отправить повторно предыдущее значение. Например счетчик был установлен в 220, произошло замыкание контакта и счетчик увеличился - значение стало 230. Триггер сработал. Затем пропала связь и счетчик выставил значение unavailable. А затем связь восстановилась и на счетчике опять появилось 230. Без not_from изменение счетчика с unavailable на 230 приведет к срабатыванию триггера. А так - нет.*

**condition**

*Проверяем условия, при которых значение датчика не является ни unknown, ни unavailable. Например, пропала связь и значение датчика изменилось на unavailable. Без этого условия триггер сработает. А так - нет.*

**action**

`- service: mqtt.publish` *сервис публикации в mqtt-топик*

`topic: /watermeter/14_412445/hotwater` *собственно топик*

`payload: "{{ as_timestamp(now()) | int }} {{ states('sensor.watermeter_ble_c3bd16_count_hot') }}"` *данные, которые отправляются в топик - таймстамп (время, когда произошло изменение состояния сенсора) и число (значение счетчика)*

Это пример. Хотя у меня именно все так и устроено. Но в секции `action` вы можете использовать свои сервисы.