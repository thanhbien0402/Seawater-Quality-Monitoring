// components/SensorAlert.tsx
"use client"

import * as React from "react"
import {
    InfoIcon,
    AlertTriangle,
    XCircleIcon,
    AlertOctagon,
} from "lucide-react"
import { Alert, AlertDescription, AlertTitle } from "@/components/ui/alert"

export type Band = "good" | "monitor" | "risk" | "severe"

export type Threshold = {
    good: [number, number]
    monitorDist: number
    riskDist: number
    severeDist: number
}

export interface SensorAlertProps {
    sensor: string
    value: number
    /** 
     * good: [idealMin, idealMax] 
     * monitorDist/riskDist/severeDist: how far outside that ideal range 
     * before escalating the band
     */
    thresholds: Threshold
}

export function AlertSensorSpecific({
    sensor,
    value,
    thresholds,
}: SensorAlertProps) {
    const { good, monitorDist, riskDist, severeDist } = thresholds
    const [minGood, maxGood] = good

    // 1) are we inside the good window?
    let band: Band
    if (value >= minGood && value <= maxGood) {
        band = "good"
    } else {
        // 2) compute distance outside the window
        const distOutside = value < minGood
            ? minGood - value
            : value - maxGood

        // 3) pick the band based on that distance
        if (distOutside <= monitorDist) {
            band = "monitor"
        } else if (distOutside <= riskDist) {
            band = "risk"
        } else {
            band = "severe"
        }
    }

    // 4) render with the appropriate colours, icon & message
    const config: Record<
        Band,
        { bg: string; Icon: React.ComponentType<any>; title: string; desc: string }
    > = {
        good: {
            bg: "bg-green-600 text-white",
            Icon: InfoIcon,
            title: `${sensor}: Good ✅`,
            desc: `${Number(value ?? 0).toFixed(2)} is within the ideal range (${minGood}–${maxGood}).`,
        },
        monitor: {
            bg: "bg-yellow-600 text-white",
            Icon: AlertTriangle,
            title: `${sensor}: Slight Deviation ⚠️`,
            desc: `${Number(value ?? 0).toFixed(2)} is just outside the ideal range.`,
        },
        risk: {
            bg: "bg-red-600 text-white",
            Icon: XCircleIcon,
            title: `${sensor}: At Risk ❌`,
            desc: `${Number(value ?? 0).toFixed(2)} is well outside the ideal range.`,
        },
        severe: {
            bg: "bg-red-800 text-white",
            Icon: AlertOctagon,
            title: `${sensor}: Severe ⛔`,
            desc: `${Number(value ?? 0).toFixed(2)} is dangerously out of range.`,
        },
    }

    const { bg, Icon, title, desc } = config[band]

    return (
        <Alert className={bg}>
            <Icon className="mr-2 h-5 w-5 stroke-white" />
            <div>
                <AlertTitle className="font-semibold">{title}</AlertTitle>
                <AlertDescription>{desc}</AlertDescription>
            </div>
        </Alert>
    )
}
