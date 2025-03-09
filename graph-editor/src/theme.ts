export const theme = {
  colors: {
    sidebar_bg: "#1a1a1a",
    bg: "#202020",
    primary: "#fff",
    nodeBg: "#2a2a2a",
    nodeColor: "#fff",
    nodeBorder: "#444",
    controlsBg: "#2a2a2a",
    controlsColor: "#fff",
    gridColor: "#999",
    handleBg: "#666",
    handleBorder: "#444",
  },
  shadows: {
    node: "0 2px 4px rgba(0,0,0,0.2)",
  },
  fonts: {
    primary: "sans-serif",
  },
  spacing: {
    nodePadding: "10px",
    borderRadius: "8px",
  },
} as const;

export type Theme = typeof theme; 