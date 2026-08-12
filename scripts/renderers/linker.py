from .base import BaseRenderer

class LinkerRenderer(BaseRenderer):
    template_name="linker.ld.j2"

    def render_linker(self, memory, output):
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
        self.render_to_file(output/f"linker.ld", memory=memory,ram_end=ram_end)
