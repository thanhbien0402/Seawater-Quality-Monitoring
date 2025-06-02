import { AlertCircle } from "lucide-react"
import { Alert, AlertDescription, AlertTitle } from "./ui/alert"
import { useTelemetry } from "./context/TelemetryProvider"

interface WaterQualityAlertProps {
    totalOutOfRange: number
    outOfRangeList: string[]
}

export function WaterQualityAlert({ totalOutOfRange, outOfRangeList }: WaterQualityAlertProps) {
    let bgClass = ""
    let iconStroke = ""
    let title = ""
    let descr = ""

    switch (totalOutOfRange) {
        case 0:
            bgClass = "bg-green-600 text-white"
            iconStroke = "stroke-white"
            title = "Water Quality: Good ✅"
            descr = "All parameters are within the ideal range."
            break
        case 1:
            bgClass = "bg-yellow-600 text-white"
            iconStroke = "stroke-white"
            title = "Slight Deviation, Monitor ⚠️"
            descr = `One parameter is out of range; the rest are normal. Out of range: ${outOfRangeList.join(", ")}.`
            break
        case 2:
        case 3:
            bgClass = "bg-red-800 text-white"
            iconStroke = "stroke-white"
            title = "Water Quality at Risk ❌"
            descr = `${totalOutOfRange} parameters are outside the standard range. Out of range: ${outOfRangeList.join(", ")}.`
            break
        case 4:
            bgClass = "bg-red-600 text-white"
            iconStroke = "stroke-white"
            title = "Severe Water Quality ⛔"
            descr = "All parameters exceed permissible limits."
            break
    }

    return (
        <Alert className={bgClass}>
            <AlertCircle className={`mr-2 ${iconStroke}`} />
            <div>
                <AlertTitle className="font-semibold">{title}</AlertTitle>
                <AlertDescription>{descr}</AlertDescription>
            </div>
        </Alert>
    )
}

export const AlertSensor = () => {
    const { pH, ORP, EC, TUR } = useTelemetry()

    const latest = <T extends { value: number }>(arr: T[]) =>
        arr.length ? arr[arr.length - 1].value : NaN

    const realPH = latest(pH)
    const realORP = latest(ORP)
    const realTUR = latest(TUR)
    const realEC = latest(EC)

    const oPH = realPH < 7.8 || realPH > 8.5
    const oORP = realORP < 200 || realORP > 300
    const oTUR = realTUR < 0 || realTUR > 10
    const oEC = realEC < 40000 || realEC > 60000

    const totalOutOfRange = [oPH, oORP, oTUR, oEC].filter(Boolean).length

    const sensors = [
        { name: "pH", value: realPH, min: 7.8, max: 8.5 },
        { name: "ORP", value: realORP, min: 200, max: 300 },
        { name: "TUR", value: realTUR, min: 0, max: 10 },
        { name: "EC", value: realEC, min: 40000, max: 60000 },
    ]

    const outOfRangeSensors = sensors
        .filter(s => s.value < s.min || s.value > s.max)
        .map(s => s.name)

    return (
        <WaterQualityAlert totalOutOfRange={totalOutOfRange} outOfRangeList={outOfRangeSensors} />
    )
}
