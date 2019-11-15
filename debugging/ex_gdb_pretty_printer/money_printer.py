import gdb

class MoneyPrinter:

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return "%f %s" % (self.val['value'], self.currency())

    def display_hint(self):
        return 'string'

    def currency(self):
        if self.val['currency'] == 0:
            return "Euro"
        elif self.val['currency'] == 1:
            return "Dollar"
        else:
            return "Unknown"

def money_lookup_function(val):
    lookup_tag = val.type.tag
    if lookup_tag == "Money":
        return MoneyPrinter(val)
    return None

gdb.pretty_printers.append(money_lookup_function)
