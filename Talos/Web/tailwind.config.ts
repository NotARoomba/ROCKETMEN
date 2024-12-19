import type { Config } from 'tailwindcss'

export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors:{ 'eerie_black': { DEFAULT: '#212529', 100: '#070808', 200: '#0e0f11', 300: '#141719', 400: '#1b1f22', 500: '#212529', 600: '#49525b', 700: '#6f7d8b', 800: '#9fa8b2', 900: '#cfd4d8' }, 'onyx': { DEFAULT: '#343a40', 100: '#0b0c0d', 200: '#15171a', 300: '#202327', 400: '#2a2f34', 500: '#343a40', 600: '#58626c', 700: '#7d8995', 800: '#a9b0b8', 900: '#d4d8dc' }, 'french_gray': { DEFAULT: '#ced4da', 100: '#242a30', 200: '#495561', 300: '#6d7f91', 400: '#9da9b5', 500: '#ced4da', 600: '#d7dce1', 700: '#e1e5e9', 800: '#ebeef0', 900: '#f5f6f8' }, 'anti-flash_white': { DEFAULT: '#f0f0f0', 100: '#303030', 200: '#606060', 300: '#909090', 400: '#c0c0c0', 500: '#f0f0f0', 600: '#f3f3f3', 700: '#f6f6f6', 800: '#f9f9f9', 900: '#fcfcfc' }, 'sea_green': { DEFAULT: '#519872', 100: '#101e17', 200: '#213d2e', 300: '#315b45', 400: '#427a5c', 500: '#519872', 600: '#70b28f', 700: '#94c5ab', 800: '#b7d8c7', 900: '#dbece3' }, 'sandy_brown': { DEFAULT: '#ff9b54', 100: '#431c00', 200: '#873800', 300: '#ca5400', 400: '#ff730e', 500: '#ff9b54', 600: '#ffae74', 700: '#ffc297', 800: '#ffd7ba', 900: '#ffebdc' }, 'amaranth': { DEFAULT: '#ce4257', 100: '#2b0b10', 200: '#561620', 300: '#812130', 400: '#ac2c3f', 500: '#ce4257', 600: '#d86778', 700: '#e28d99', 800: '#ebb3bb', 900: '#f5d9dd' } }

    },
  },
  plugins: [],
} satisfies Config

