
from .context import GenerationContext, PeripheralContext
from fnmatch import fnmatch

def _find_clock(gen, p):
    if gen.rcc is None:
        return None,None
    target=p.name+"EN"
    for i in range(2):
        if i == 1 and target.startswith("GPIO"):
            target = target.replace("GPIO", "IOP")
        for reg in gen.rcc.registers or []:
            if not reg.name.endswith("ENR"):
                continue
            for field in reg.fields or []:
                if field.name==target:
                    return reg.name, field.bit_offset
    return None,None

def _find_clock_tag(gen, p):
    if gen.rcc is None:
        return None,None
    target=p.name+"EN"
    for i in range(2):
        if i == 1 and target.startswith("GPIO"):
            target = target.replace("GPIO", "IOP")
        for reg in gen.rcc.registers or []:
            if not reg.name.endswith("ENR"):
                continue
            for field in reg.fields or []:
                if field.name==target:
                    return reg.name[:-3]
    return None

def _find_reset(gen,p):
    if gen.rcc is None:
        return None,None
    target=p.name+"RST"
    for i in range(2):
        if i == 1 and target.startswith("GPIO"):
            target = target.replace("GPIO", "IOP")
        for reg in gen.rcc.registers or []:
            if not reg.name.endswith("RSTR"):
                continue
            for field in reg.fields or []:
                if field.name==target:
                    return reg.name, field.bit_offset
    return None,None

def get_signals(peripheral, funs):
    return [p for p in funs if p["peripheral"] == peripheral.name]

def get_config(signal: str, rules, return_none: bool = False) -> str:
    if signal in rules:
        return rules[signal]

    patterns = sorted(
        ((p, c) for p, c in rules.items() if any(ch in p for ch in "*?[")),
        key=lambda x: len(x[0]),
        reverse=True,
    )

    for pattern, config in patterns:
        if fnmatch(signal, pattern):
            return config
    if return_none:
        return None
    raise KeyError(signal)

def fix_config(conf):
    if conf.startswith("Alternate"):
        return f"{conf}<0>"
    return conf

def make_signal(peripheral, pin, funs, signals_config):
    signals = get_signals(peripheral, funs)
    if len(signals) == 0:
        return {}
    signal = signals[0]
    signal = signal["signal"]
    return {
        "name": signal.lower().capitalize(),
        "port": pin[1],
        "pin": pin[2:],
        "config": fix_config(get_config(signal, signals_config["signals"])["config"])
    }


def make_signals(af, peripheral, signals_config):
    pins = [make_signal(peripheral, f, af[f]["functions"], signals_config) for f in af if len(get_signals(peripheral, af[f]["functions"])) > 0]
    return pins

def find_bits_in_reg(peripheral, bits, return_list: bool = False):
    if not isinstance(bits, list):
        bits = [bits]
    ret = []
    for bit in bits:
        for reg in peripheral.registers:
            for field in reg.fields:
                if field.name == bit:
                    ret.append(f"{reg.name}::{bit}")
    if not return_list and len(ret) == 1:
        return ret[0]
    return ret



def make_event(peripheral, event, config):
    event_cfg = config[event]
    clear_policy = event_cfg["clear"] if "clear" in event_cfg else "None"
    return {
        "name": event,
        "clear_policy": clear_policy,
        "status_bit": find_bits_in_reg(peripheral, event_cfg["status"], True),
        "data_bit": event_cfg["data"] if "data" in event_cfg else "void",
        "enable_bit": find_bits_in_reg(peripheral, event_cfg["enable"])
    }

def make_events(af, peripheral, event_config):
    config = get_config(peripheral.name, event_config, True)
    if config is None:
        return
    ret = [make_event(peripheral, c, config) for c in config]
    
    return [r for r in ret if len(r["enable_bit"]) > 0 and len(r["status_bit"]) > 0]

def make_context(gen:GenerationContext, signals_config, event_config, af, peripheral):
    ctx=PeripheralContext(
        generation=gen,
        peripheral=peripheral,
        is_gpio=peripheral.name.startswith("GPIO"),
        is_rcc=peripheral.name=="RCC",
        is_uart=peripheral.name.startswith("UART") or peripheral.name.startswith("USART"),
        is_spi=peripheral.name.startswith("SPI"),
        is_i2c=peripheral.name.startswith("I2C"),
        is_timer=peripheral.name.startswith("TIM")
    )
    ctx.event_namespace = "spi" if ctx.is_spi else "uart" if ctx.is_uart else "i2c" if ctx.is_i2c else "timer" if ctx.is_timer else "gpio"
    ctx.traits_namespace = "spi" if ctx.is_spi else "uart" if ctx.is_uart else "i2c" if ctx.is_i2c else "timer" if ctx.is_timer else "gpio::port"
    ctx.signals = make_signals(af, peripheral, signals_config)
    ctx.events = make_events(af, peripheral, event_config)
    ctx.gpio_inc = list(set([signal["port"].lower() for signal in ctx.signals]))
    if ctx.is_gpio:
        ctx.gpio_policy = "STM32F1Policy" if gen.device.name.startswith("STM32F1") else "STM32V2Policy"
    ctx.clock_register,ctx.clock_bit=_find_clock(gen,peripheral)
    ctx.clock_tag=_find_clock_tag(gen,peripheral)
    ctx.reset_register,ctx.reset_bit=_find_reset(gen,peripheral)

    if peripheral.interrupts:
        ctx.irq_number=peripheral.interrupts[0].value
    return ctx
