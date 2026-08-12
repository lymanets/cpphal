from .base import BaseRenderer

class VectorsRenderer(BaseRenderer):
    template_name="vectors.cpp.j2"

    def render_vectors(self, device, irq, output):
        max_irq = max(i for i in irq)

        vectors = [None] * (max_irq + 1)

        for i in irq:
            vectors[i] = irq[i][0]

        self.render_to_file(output/"vectors.cpp",
                            vectors=vectors,
                            device_name=device.name)
