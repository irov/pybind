def enum_rejects(function, value):
    try:
        function(value)
    except Exception:
        return True
    return False

enum_value_ok = SIGNED_NATIVE_UNKNOWN == -1 and SIGNED_NATIVE_READY == 1 and UNSIGNED_NATIVE_HIGH == 2147483648 and SIGNED_WIDE_LOW == -1099511627776 and UNSIGNED_WIDE_HIGH == 18446744073709551615

enum_roundtrip_ok = roundtrip_signed_native(SIGNED_NATIVE_UNKNOWN) == SIGNED_NATIVE_UNKNOWN and roundtrip_signed_native(SIGNED_NATIVE_READY) == SIGNED_NATIVE_READY and roundtrip_unsigned_native(UNSIGNED_NATIVE_HIGH) == UNSIGNED_NATIVE_HIGH and roundtrip_signed_wide(SIGNED_WIDE_LOW) == SIGNED_WIDE_LOW and roundtrip_unsigned_wide(UNSIGNED_WIDE_HIGH) == UNSIGNED_WIDE_HIGH

enum_overflow_ok = enum_rejects(roundtrip_signed_native, 2 ** 64) and enum_rejects(roundtrip_unsigned_native, 2 ** 64) and enum_rejects(roundtrip_signed_wide, 2 ** 64) and enum_rejects(roundtrip_unsigned_wide, 2 ** 64)
