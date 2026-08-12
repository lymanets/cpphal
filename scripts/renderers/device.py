from .base import BaseRenderer
import re

def cmsis_cpu_revision(revision: str) -> int:
    m = re.fullmatch(r"r(\d+)p(\d+)", revision)

    if not m:
        raise ValueError(f"Invalid CPU revision: {revision}")

    r, p = map(int, m.groups())
    return (r << 8) | p

class DeviceRenderer(BaseRenderer):
    template_name="device.hpp.j2"

    def render_device(self, device, freq: dict, namespace, output):
        freqs = [{"name": k, "value": freq[k] * 1000000} for k in freq.keys()]
        self.render_to_file(output/"device.hpp",
                            device = device,
                            device_family = device.name[:-2],
                            cpu_rev=f"0x{cmsis_cpu_revision(device.cpu.revision):04X}",
                            freqs = freqs,
                            cpu_header=device.cpu.name.value.lower(),
                            namespace = namespace)
