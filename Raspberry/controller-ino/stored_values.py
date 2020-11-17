###################################################################################################
# Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        #
# You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    #
# You can find a copy of the license inside the LICENSE file you received with this code          #
# (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     #
# or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       #
###################################################################################################

import json
import math
import random

class Range:
    def __init__(self, values):
        self.__values = values


    def closest(self, value):
        return min(self.__values, key=lambda x: abs(x - value))


    def increment(self, value):
        if value >= max(self.__values):
            return max(self.__values)
        return min(self.__values, key=lambda x:(math.inf, x - value)[x - value > 0])


    def decrement(self, value):
        if value <= min(self.__values):
            return min(self.__values)
        return max(self.__values, key=lambda x:(-math.inf, x - value)[x - value < 0])

    def rand(self):
        return random.choice(self.__values)


    def __str__(self):
        return str(self.__values)


    def index(self, element):
        return self.__values.index(element)

    def get_min(self):
        return min(self.__values)

    def get_max(self):
        return max(self.__values)


class CircularRange(Range):
    def __init__(self, values, bounds):
        for v in values:
            if v<bounds[0] or v>=bounds[1]:
                raise ValueError('Possible value {} outside boundaries [{}, {})'.format(v, bounds[0], bounds[1]))

        super().__init__(values)
        self.bounds = bounds

    def closest(self, value):
        value = (value-self.bounds[0])%(self.bounds[1]-self.bounds[0])+self.bounds[0]
        return super().closest(value)

    def increment(self, value):
        if value >= super().get_max():
            return super().get_min()
        return super().increment(value)


    def decrement(self, value):
        if value <= super().get_min():
            return super().get_max()
        return super().decrement(value)

    def rand(self):
        return super().rand()

    def __str__(self):
        return super().__str__()


class StoredValues:
    ranges = {
        "chamber_hue":          CircularRange([(4*i) for i in range(64)], (0, 256)),
        "chamber_saturation":   Range([(17*i) for i in range(16)]),
        "chamber_value":        Range([(8*i)+7 for i in range(32)]),
        "effectDuration":       Range([((i, 4*(i-12))[i>=16]) for i in range(32)]),
        "fadingDuration":       Range([i for i in range(8)]),
        "extruder_hue":         CircularRange([(4 * i) for i in range(64)], (0, 256)),
        "extruder_saturation":  Range([(17 * i) for i in range(16)]),
        "extruder_value":       Range([(8 * i) + 7 for i in range(32)]),
        "chamberLights":        Range([i for i in range(2)]),
        "extruderLights":       Range([i for i in range(2)]),
        "lightsMode":           Range([i for i in range(28)]),
        "fadingMode":           Range([i for i in range(32)])
    }


    def __init__(self, sv_path, load_from_file=True):
        self.__path = sv_path

        self.chamber_hue = 0
        self.chamber_saturation = 0
        self.chamber_value = 255
        self.effectDuration = 20
        self.fadingDuration = 4
        self.extruder_hue = 0
        self.extruder_saturation = 0
        self.extruder_value = 63

        self.chamberLights = 1
        self.extruderLights = 1
        self.lightsMode = 0
        self.fadingMode = 0

        self.reversibleChanges = {}

        if load_from_file:
            self.load_values()


    def reset_values(self):
        self.__init__(sv_path=self.__path, load_from_file=False)


    def load_values(self):
        try:
            with open(self.__path, 'r') as sv_file:
                data = json.load(sv_file)
                self.chamber_hue = data['chamber_hue']
                self.chamber_saturation = data['chamber_saturation']
                self.chamber_value = data['chamber_value']

                self.effectDuration = data['effectDuration']
                self.fadingDuration = data['fadingDuration']

                self.extruder_hue = data['extruder_hue']
                self.extruder_saturation = data['extruder_saturation']
                self.extruder_value = data['extruder_value']

        except Exception as exc:
            print(exc)
            print("Resetting stored values...")
            self.store_values()


    def store_values(self):
        try:
            with open(self.__path, 'w') as sv_file:
                json.dump(self.__dict__, sv_file)
        except Exception as exc:
            print(exc)


    def increment_value(self, attribute_name, increment, is_reversible=False):
        if attribute_name not in self.ranges:
            raise AttributeError('Unknown attribute {}'.format(attribute_name))

        is_increment = True
        if increment < 0:
            is_increment = False
            increment *= -1

        rng = self.ranges[attribute_name]
        v = getattr(self, attribute_name)

        if is_reversible:
            self.reversibleChanges[attribute_name] = v

        for i in range(increment):
            v = (rng.decrement(v), rng.increment(v))[is_increment]

        setattr(self, attribute_name, v)

        return rng.index(v), v


    def set_value(self, attribute_name, value, is_reversible=False):
        if attribute_name not in self.ranges:
            raise AttributeError('Unknown attribute {}'.format(attribute_name))

        rng = self.ranges[attribute_name]

        if is_reversible:
            self.reversibleChanges[attribute_name] = getattr(self, attribute_name)

        v = rng.closest(value)
        setattr(self, attribute_name, v)

        return rng.index(v), v


    def revert_changes(self):
        ret = []
        for attr in self.reversibleChanges:
            v = self.reversibleChanges[attr]
            rng = self.ranges[attr]
            ret.append((attr, rng.index(v), v))

        self.reversibleChanges = {}
        return ret

    def get_random(self, attribute_name):
        if attribute_name not in self.ranges:
            raise AttributeError('Unknown attribute {}'.format(attribute_name))

        return self.ranges[attribute_name].rand()
