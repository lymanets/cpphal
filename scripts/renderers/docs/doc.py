from scripts.renderers.base import BaseRenderer
import re

def add_space_before_capitals(text: str) -> str:
    r = re.sub(r'(?<!^)(?=[A-Z])', ' ', text)
    r =[i.capitalize() for i in r.split("_")]
    return " ".join(r)


class DocRenderer(BaseRenderer):

    def __init__(self, env, name, event, ):
        self.template_name=f"docs/{name}.md.j2"
        self.name = name
        self.event = event
        super().__init__(env)

    def render_doc(self, events, peripheral_options, doc_dir):
        p_events = events[self.event]
        p_options = peripheral_options[self.name]["options"]
        for v in p_options:
            v["name_with_space"] = add_space_before_capitals(v["name"])
        self.render_to_file(doc_dir/f"{self.name}.md",
                            events=p_events.keys(),
                            options=p_options,
                            )
