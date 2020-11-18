###################################################################################################
# Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        #
# You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    #
# You can find a copy of the license inside the LICENSE file you received with this code          #
# (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     #
# or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       #
###################################################################################################

from stored_values import StoredValues

class ArduinoMessage:
    __messages = {
        "chamberLights": (0, 1),
        "extruderLights": (2, 3),
        "lightsMode": (4, 31),
        "value": (32, 63),
        "hue": (64, 127),
        "status": (128, 143),
        "revertLightsMode": (144, 144),
        "resetSettings": (145, 145),
        "loadSettings": (146, 146),
        "storeSettings": (147, 147),
        "scheduleReboot": (148, 149),
        "scheduleShutdown": (150, 151),
        "fadingDuration": (152, 159),
        "effectDuration": (160, 191),
        "saturation": (192, 207),
        "keepRaspberryOn": (208, 209),
        "chamber": (210, 210),
        "extruder": (211, 211),
        "fadingMode": (224, 255)
    }

    @staticmethod
    def get_message(message_name, value):
        if message_name not in ArduinoMessage.__messages:
            raise AttributeError('Unknown attribute {}'.format(message_name))

        rng = ArduinoMessage.__messages[message_name]
        m = rng[0] + value

        if m > rng[1] or m < rng[0]:
            raise ValueError('Value {} causes message {} go out of bounds'.format(value, m))

        return m


class DuetMessage:
    def __init__(self, stored_values):
        self.stored_values = stored_values
        self.last_var = ''


    def __assign(self, variable, value):
        try:
            needs_backup = False
            if variable.endswith(self.__backup_modifier):
                needs_backup = True
                variable = variable.replace(self.__backup_modifier, '')

            if variable == 'last':
                variable = self.last_var

            if value == 'rand':
                value = self.stored_values.get_random(variable)
            else:
                value = int(value)

            i, v = self.stored_values.set_value(variable, value, is_reversible=needs_backup)
            self.last_var = variable
            if DuetMessage.__lights_separator in variable:
                variable = variable.split(DuetMessage.__lights_separator)[1]
            return ArduinoMessage.get_message(variable, i), 'Set {} to {}'.format(variable, v)
        except Exception as exc:
            print(exc, flush=True)
            return -1, 'Failed to set {} to {}'.format(variable, value)

    def __increase(self, variable, value):
        try:
            needs_backup = False
            if variable.endswith(self.__backup_modifier):
                needs_backup = True
                variable = variable.replace(self.__backup_modifier, '')

            if variable == 'last':
                variable = self.last_var

            if value == 'rand':
                return self.__assign(variable, value)

            i, v = self.stored_values.increment_value(variable, int(value), is_reversible=needs_backup)
            self.last_var = variable
            if DuetMessage.__lights_separator in variable:
                variable = variable.split(DuetMessage.__lights_separator)[1]
            return ArduinoMessage.get_message(variable, i), 'Increase {} to {}'.format(variable, v)
        except Exception as exc:
            print(exc, flush=True)
            return -1, 'Failed to increase {} {} times'.format(variable, value)

    def __decrease(self, variable, value):
        try:
            needs_backup = False
            if variable.endswith(self.__backup_modifier):
                needs_backup = True
                variable = variable.replace(self.__backup_modifier, '')

            if variable == 'last':
                variable = self.last_var

            if value == 'rand':
                return self.__assign(variable, value)

            i, v = self.stored_values.increment_value(variable, -int(value), is_reversible=needs_backup)
            self.last_var = variable
            if DuetMessage.__lights_separator in variable:
                variable = variable.split(DuetMessage.__lights_separator)[1]
            return ArduinoMessage.get_message(variable, i), 'Decrease {} to {}'.format(variable, v)
        except Exception as exc:
            print(exc, flush=True)
            return -1, 'Failed to decrease {} {} times'.format(variable, value)

    __separators = [',', ';']

    __operators = {
        ':=': __assign,
        '+=': __increase,
        '-=': __decrease
    }

    __backup_modifier = '^'

    __anyLight_modifier = '*'

    __lights_separator = '_'

    def __revert_lights(self):
        commands = ''
        to_be_reverted = self.stored_values.revert_changes()
        if len(to_be_reverted) == 0:
            return -1, "There is nothing to be reverted"

        for variable, i, v in to_be_reverted:
            try:
                if variable == 'lightsMode':
                    commands=commands+'revertLightsMode, '
                else:
                    commands=commands+'{}:={}, '.format(variable, v)
            except Exception as exc:
                print(exc, flush=True)
        return self.handle_message(commands[:len(commands)-2])

    def __settings(self, command):
        try:
            getattr(self.stored_values, command+'_values')()
            return ArduinoMessage.get_message(command+'Settings', 0), '{} settings'.format(command)
        except Exception as exc:
            print(exc, flush=True)
            return -1, 'Failed to {} settings'.format(command)

    def __arduino(self, command, value):
        try:
            return ArduinoMessage.get_message(command, value), 'Call {} function with value {}'.format(command, value)
        except Exception as exc:
            print(exc, flush=True)
            return -1, 'Failed to call {} function with value {}'.format(command, value)


    __functions = {
        "chamberLightsOff": (__assign, 'chamberLights', 0),
        "chamberLightsOn": (__assign, 'chamberLights', 1),
        "extruderLightsOff": (__assign, 'extruderLights', 0),
        "extruderLightsOn": (__assign,  'extruderLights', 1),
        "revertLights": (__revert_lights,),
        "resetSettings": (__settings, 'reset'),
        "loadSettings": (__settings, 'load'),
        "storeSettings": (__settings, 'store'),
        "scheduleReboot": (__arduino, 'scheduleReboot', 1),
        "unScheduleReboot": (__arduino, 'scheduleReboot', 0),
        "scheduleShutdown": (__arduino, 'scheduleShutdown', 1),
        "unScheduleShutdown": (__arduino, 'scheduleShutdown', 0),
        "keepRaspberryOn": (__arduino, 'keepRaspberryOn', 1),
        "keepRaspberryOff": (__arduino, 'keepRaspberryOn', 0),
        "chamber": (__arduino, 'chamber', 0),
        "extruder": (__arduino, 'extruder', 0)
    }

    def __function(self, command):
        try:
            f = DuetMessage.__functions[command]
            if len(f) == 1:
                return f[0](self)
            elif len(f) == 2:
                return f[0](self, f[1])
            elif len(f) == 3:
                return f[0](self, f[1], f[2])
            else:
                raise ValueError("Tuple returned with command {} has an unknown number of parameters ({})".format(command, len(f)))

        except Exception as exc:
            print(exc, flush=True)
            return -1, "Failed to call {} command".format(command)

    def handle_message(self, message=''):
        ret = []
        message = ''.join(message.split())

        for i in range(1, len(DuetMessage.__separators)):
            message = message.replace(DuetMessage.__separators[i], DuetMessage.__separators[0])
        commands = message.split(DuetMessage.__separators[0])

        lights = ''
        i=0
        for c in commands:
            if c.startswith(self.__anyLight_modifier+self.__lights_separator):
                first_light = (lights, 'chamber')[lights=='']
                second_light = ('chamber', 'extruder')[first_light=='chamber']
                commands.insert(i+1, c.replace(self.__anyLight_modifier, second_light))
                c = c.replace(self.__anyLight_modifier, first_light)

            op_found = False
            for op in DuetMessage.__operators:
                if not op_found and op in c:
                    op_found=True
                    if DuetMessage.__lights_separator in c:
                        sc = c.split(DuetMessage.__lights_separator)
                        if sc[0] != lights:
                            lights = sc[0]
                            ret.append(DuetMessage.__function(self, lights))
                    v = c.split(op)
                    ret.append(DuetMessage.__operators[op](self, v[0], v[1]))
            if not op_found and c in DuetMessage.__functions:
                res = DuetMessage.__function(self, c)
                if type(res) is list:
                    ret.append((256, 'Starting {}'.format(c)))
                    ret.extend(res)
                    ret.append((256, 'Ending {}'.format(c)))
                elif type(res) is tuple and len(res) == 2:
                    ret.append(res)
                else:
                    ret.append((-1, 'Result of function {} is unknown: {}'.format(c, str(res))))
            elif not op_found:
                ret.append((-1, 'Unknown function named {}'.format(c)))
            i+=1
        return ret

########################################################################################################################
# Operators:                                                                                                           #
#   :=  assign      a:=n                                                                                               #
#   +=  increase    a+=n    increase attribute a n times                                                               #
#   -=  decrease    a-=n    decrease attribute a n times                                                               #
#                                                                                                                      #
# Variables:                                                                                                           #
#   last                    last modified variable                                                                     #
#   chamber_hue             chamber lights' hue                                                                        #
#   chamber_saturation      chamber lights' saturation                                                                 #
#   chamber_value           chamber lights' brightness                                                                 #
#   extruder_hue            extruder lights' hue                                                                       #
#   extruder_saturation     extruder lights' saturation                                                                #
#   extruder_value          extruder lights' brightness                                                                #
#   effectDuration          time after which a new random effect is chosen (0 = infinite)                              #
#   fadingDuration          duration of fading between effects (0 = no fading)                                         #
#   chamberLights           whether chamber lights are ON (1) or OFF (0)                                               #
#   extruderLights          whether extruder lights are ON (1) or OFF (0)                                              #
#   lightsMode              light effects to be displayed during print                                                 #
#   fadingMode              fading performed between effects change                                                    #
#                                                                                                                      #
# Modifiers:                                                                                                           #
#   ^   backup var  a^:=n   before editing the given variable, backup it for future revert                             #
#   *   any light   *_a:=n  edit the given hue, saturation or value both for chamber and extruder lights               #
#                                                                                                                      #
# Functions:                                                                                                           #
#   chamberLightsOff        switch off chamber lights                                                                  #
#   chamberLightsOn         switch on chamber lights                                                                   #
#   extruderLightsOff       switch off extruder lights                                                                 #
#   extruderLightsOn        switch on extruder lights                                                                  #
#   revertLights            revert settings to backed-up values                                                        #
#   resetSettings           reset settings to default                                                                  #
#   loadSettings            load settings stored on EEPROM                                                             #
#   storeSettings           store actual settings on EEPROM                                                            #
#   scheduleReboot          schedule reboot as soon as printer reaches idle state                                      #
#   unScheduleReboot        abort scheduled reboot                                                                     #
#   scheduleShutdown        schedule shutdown as soon as printer reaches idle state                                    #
#   unScheduleShutdown      abort scheduled shutdown                                                                   #
#   keepRaspberryOn         keep raspberry on when duet is off                                                         #
#   keepRaspberryOff        keep raspberry off when duet is off                                                        #
#   chamber                 following operations will update chamber HSV                                               #
#   extruder                following operations will update extruder HSV                                              #
#                                                                                                                      #
# Separators:                                                                                                          #
#   ,                       separator between instructions                                                             #
#   ;                       separator between instructions                                                             #
#                                                                                                                      #
########################################################################################################################

if __name__ == '__main__':
    sv = StoredValues('.storedValuesTest.json')
    dm = DuetMessage(sv)
    print(dm.handle_message('*_hue^:=rand, *_saturation^:=255'))
    print(dm.handle_message('chamber_saturation-=2, revertLights, chamberLightsOff, extruder_saturation:=0'))
