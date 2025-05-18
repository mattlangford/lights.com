import React from "react";
import { theme } from "../theme";

interface SidebarNodeProps {
  label: string;
  config: Record<string, number | boolean | string>;
}

const SidebarNode: React.FC<SidebarNodeProps> = ({ label, config }) => {
  const inputs = typeof config.inputs === 'number' ? config.inputs : 0;
  const outputs = typeof config.outputs === 'number' ? config.outputs : 0;

  return (
    <div
      style={{
        padding: theme.spacing.nodePadding,
        borderRadius: theme.spacing.borderRadius,
        background: theme.colors.nodeBg,
        border: `1px solid ${theme.colors.nodeBorder}`,
        textAlign: "center",
        minWidth: "150px",
        position: "relative",
        color: theme.colors.nodeColor,
        fontFamily: theme.fonts.primary,
        boxShadow: theme.shadows.node,
      }}
    >
      {/* Fake Input Handles */}
      {Array.from({ length: Number(inputs) }).map((_, index) => (
        <div
          key={`input-${index}`}
          style={{
            position: "absolute",
            left: "-5px",
            top: `${(index + 1) * (100 / (Number(inputs) + 1))}%`,
            width: "10px",
            height: "10px",
            borderRadius: "50%",
            background: theme.colors.handleBg,
            border: `2px solid ${theme.colors.handleBorder}`,
            transform: "translateY(-50%)",
          }}
        />
      ))}

      <div>{label}</div>

      {/* Fake Output Handles */}
      {Array.from({ length: Number(outputs) }).map((_, index) => (
        <div
          key={`output-${index}`}
          style={{
            position: "absolute",
            right: "-5px",
            top: `${(index + 1) * (100 / (Number(outputs) + 1))}%`,
            width: "10px",
            height: "10px",
            borderRadius: "50%",
            background: theme.colors.handleBg,
            border: `2px solid ${theme.colors.handleBorder}`,
            transform: "translateY(-50%)",
          }}
        />
      ))}
    </div>
  );
};

export default SidebarNode; 