from scripts.renderers.base import BaseRenderer
import re

def cmsis_cpu_revision(revision: str) -> int:
    m = re.fullmatch(r"r(\d+)p(\d+)", revision)

    if not m:
        raise ValueError(f"Invalid CPU revision: {revision}")

    r, p = map(int, m.groups())
    return (r << 8) | p

class DeviceRenderer(BaseRenderer):
    template_name="headers/device.hpp.j2"

    def render_device(self, device, freq: dict, namespace, irq_count, memory, output):
        freqs = [{"name": k, "value": freq[k] * 1000000} for k in freq.keys()]
        ram_end = None
        ram_start = None
        for m in memory:
            if m["name"] == "RAM":
                ram_end = m["length"].replace("K", "")
                ram_start = m["origin"]
        if ram_end is None or ram_start is None:
            raise ValueError(f"RAM is not defined")
        ram_end = (int(ram_end) * 1024) - 1
        ram_end += ram_start
        ram_end = f"0x{ram_end:0X}"
        self.render_to_file(output/"device.hpp",
                            device = device,
                            device_family = device.name[:-2],
                            cpu_rev=f"0x{cmsis_cpu_revision(device.cpu.revision):04X}",
                            freqs = freqs,
                            irq_count = irq_count,
                            stack_top=ram_end,
                            cpu_header = device.cpu.name.value.lower(),
                            namespace = namespace)
