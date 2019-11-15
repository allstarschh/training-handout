# naming convention:
# - start with `qdump__`
# - append qualified identifier of C++ type
# - replace `::` with `__`
def qdump__Money(d, value):
    currency = "unknown"
    if value["currency"] == 0:
        currency = "Euro"
    elif value["currency"] == 1:
        currency = "Dollar"

    d.putNumChild(0)
    d.putValue("%f %s" % (value["value"], currency))
