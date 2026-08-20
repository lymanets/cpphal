from scripts.renderers.base import BaseRenderer

def count_timer_channels(peripheral):
    channels = 0

    for channel in range(1, 5):
        if any(
        register.name == f"CCR{channel}"
        for register in peripheral.registers
        ):
            channels += 1

    return channels

def has_bdtr(peripheral):
    return any(
        register.name == "BDTR"
        for register in peripheral.registers
    )

def timer_info(peripheral):
    channels = count_timer_channels(peripheral)

    if channels == 0:
        timer_type = "Basic"
    elif has_bdtr(peripheral):
        timer_type = "Advanced"
    else:
        timer_type = "GeneralPurpose"

    return timer_type, channels

class PeripheralRenderer(BaseRenderer):
    template_name="headers/peripheral.hpp.j2"

    def render_peripheral(self, ctx, namespace, output):
        timer_type, channels = timer_info(ctx.peripheral)
        self.render_to_file(output/f"{ctx.peripheral.name.lower()}.hpp",
                            ctx=ctx,
                            timer_type=timer_type, 
                            channels=channels,
                            namespace=namespace)
