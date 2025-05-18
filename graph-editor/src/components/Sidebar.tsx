import React, { DragEvent } from "react";
import nodeConfigs from "../nodeConfigs";
import { theme } from "../theme";
import SidebarNode from "./SidebarNode";

const Sidebar: React.FC = () => {
  const onDragStart = (event: DragEvent<HTMLDivElement>, nodeType: string) => {
    // Create a clone of the node for the drag preview
    const dragEl = event.currentTarget.cloneNode(true) as HTMLDivElement;
    dragEl.style.position = 'fixed';
    dragEl.style.top = '-1000px';
    dragEl.style.left = '-1000px';
    document.body.appendChild(dragEl);
    
    // Set the drag image to be the cloned node
    event.dataTransfer.setDragImage(dragEl, dragEl.offsetWidth / 2, dragEl.offsetHeight / 2);
    event.dataTransfer.setData("application/reactflow", JSON.stringify({ id: nodeType, ...nodeConfigs[nodeType] }));
    event.dataTransfer.effectAllowed = "move";
    
    // Remove the clone after the drag starts
    setTimeout(() => {
      document.body.removeChild(dragEl);
    }, 0);
  };

  return (
    <aside style={{
      padding: 10,
      width: 200,
      background: theme.colors.sidebar_bg,
      color: theme.colors.primary,
      fontFamily: theme.fonts.primary
    }}>
      <h3>Nodes</h3>
      {Object.keys(nodeConfigs).map((key) => (
        <div
          key={key}
          draggable
          onDragStart={(event) => onDragStart(event, key)}
          style={{
            margin: "10px 0",
            cursor: "grab",
          }}
        >
          <SidebarNode
            label={nodeConfigs[key].name}
            config={nodeConfigs[key].config}
          />
        </div>
      ))}
    </aside>
  );
};

export default Sidebar;

