import { clsx, type ClassValue } from "clsx"
import { twMerge } from "tailwind-merge"

export function cn(...inputs: ClassValue[]) {
  return twMerge(clsx(inputs))
}

const EPOCH = 1672531200000

let lastTs = 0
let seq = 0

/**
 * Sinh số ID duy nhất, monotonic, collision-resistant
 */
export function generateUniqueNumber(): number {
  let now = Date.now()

  if (now === lastTs) {
    // trong cùng ms, tăng sequence
    seq = (seq + 1) & 0xfff   // 0xfff = 4095 (12 bit)
    if (seq === 0) {
      // quá 4095 lần trong cùng ms, đợi sang ms mới
      while (Date.now() <= lastTs) { }
      now = Date.now()
    }
  } else {
    // qua ms mới, reset sequence
    seq = 0
  }

  lastTs = now

  // shift timestamp xuống 12 bit, OR với seq
  // (now - EPOCH) đảm bảo value nhỏ hơn 2^41
  const id =
    ((now - EPOCH) << 12) | seq

  return id
}

export const formatDate = (isContainingDate: boolean, timestamp?: number) => {
  const date = timestamp ? new Date(timestamp) : new Date();

  // Array of abbreviated weekday names
  const daysOfWeek = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];
  const dayName = daysOfWeek[date.getDay()];

  // Get day, month, and full year
  const day = ("0" + date.getDate()).slice(-2);
  const month = ("0" + (date.getMonth() + 1)).slice(-2); // Months are 0-indexed!
  const year = date.getFullYear();

  // Get hours, minutes, and seconds with leading zeros
  const hours = ("0" + date.getHours()).slice(-2);
  const minutes = ("0" + date.getMinutes()).slice(-2);
  const seconds = ("0" + date.getSeconds()).slice(-2);

  // Combine everything into the desired format
  const formattedDateTime = isContainingDate ? (`${day}/${month}/${year}` + ` ${hours}:${minutes}:${seconds}`) : "" + `${hours}:${minutes}:${seconds}`;

  // console.log(formattedDateTime);

  return formattedDateTime
}

